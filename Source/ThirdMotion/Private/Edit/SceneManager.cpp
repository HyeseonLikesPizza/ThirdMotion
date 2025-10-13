
#include "Edit/SceneManager.h"

#include "EngineUtils.h"
#include "GameplayTagContainer.h"
#include "Edit/AssetResolver.h"
#include "Edit/EditSyncComponent.h"
#include "ThirdMotion/ThirdMotion.h"

void USceneManager::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);
	GuidCache.Empty();
}

void USceneManager::Deinitialize()
{
	UnbindWorldDelegates();
	GuidCache.Empty();

	Super::Deinitialize();
}

void USceneManager::OnWorldBeginPlay(UWorld& InWorld)
{
	Super::OnWorldBeginPlay(InWorld);

	if (auto* R = InWorld.GetSubsystem<UAssetResolver>())
	{
		if (R->IsReady()) OnResolverReady();
		else R->OnReady.AddUObject(this, &USceneManager::OnResolverReady);
	}
	else
	{
		PRINTLOG(TEXT("AssetResolver not found"));
	}

	if (IsAuthority())
	{
		BuildInitialGuidCache();
		BindWorldDelegates();
	}
}

bool USceneManager::IsAuthority() const
{
	const UWorld* W = GetWorld();
	return W && W->GetNetMode() != NM_Client;
}

AActor* USceneManager::FindByGuid(const FGuid& ID) const
{
	if (const TWeakObjectPtr<AActor>* Found = GuidCache.Find(ID))
		return Found->Get();
	return nullptr;
}

bool USceneManager::DestroyByGuid(const FGuid& ID)
{
	check(IsAuthority());

	if (AActor* Actor = FindByGuid(ID))
	{
		GuidCache.Remove(ID);
		Actor->Destroy();
		return true;
	}
	return false;
}

AActor* USceneManager::SpawnByTag(FGameplayTag PresetTag, const FTransform& T)
{
	check(IsAuthority());

	auto* Resolver = GetWorld()->GetSubsystem<UAssetResolver>();
	if (!Resolver || !Resolver->IsReady()) return nullptr;

	const FLibraryRow* Row = Resolver->FindRowByTag(PresetTag);
	if (!Row) return nullptr;

	TSoftClassPtr<AActor> SoftClass = Row->ClassRef;
	if (SoftClass.IsNull()) return nullptr;

	UClass* ClassToSpawn = SoftClass.LoadSynchronous();
	if (!ClassToSpawn) return nullptr;

	AActor* NewActor = GetWorld()->SpawnActorDeferred<AActor>(ClassToSpawn, T);
	if (!NewActor) return nullptr;

	AttachEditComponentAndMeta(NewActor, *Row);

	// 편집용 메타/컴포넌트 부착, GUID 발급 등 ...
	NewActor->SetReplicates(true);
	NewActor->SetReplicateMovement(true);
	NewActor->FinishSpawning(T);

	if (UEditSyncComponent* Edit = NewActor->FindComponentByClass<UEditSyncComponent>())
		GuidCache.FindOrAdd(Edit->GetMeta().Guid) = NewActor;
	
	return NewActor;
}

bool USceneManager::ApplyTransform(const FGuid& ID, const FTransform& T)
{
	check(IsAuthority());

	if (AActor* Actor = FindByGuid(ID))
	{
		FTransform Transform = T;
		Actor->SetReplicateMovement(true);
		Actor->SetActorTransform(Transform, true, nullptr, ETeleportType::None);
		return true;
	}
	return false;
}

bool USceneManager::ApplyPropertyDelta(const FGuid& Guid, const FPropertyDelta& Delta)
{
	check(IsAuthority());

	if (AActor* Actor = FindByGuid(Guid))
	{
		if (UEditSyncComponent* Edit = Actor->FindComponentByClass<UEditSyncComponent>())
		{
			// 서버 즉시 적용 + 히스토리 추가 -> 복제로 클라 반영
			Edit->ServerApplyPropertyDelta_Internal(Delta);
			return true;
		}
	}
	return false;
}


/* -------------- 내부 유틸 --------------- */

void USceneManager::AttachEditComponentAndMeta(AActor* Actor, const struct FLibraryRow& Row)
{
	auto* Edit = NewObject<UEditSyncComponent>(Actor);
	Edit->RegisterComponent();
	Edit->InitMetaFromPreset(Row);
}

void USceneManager::OnResolverReady()
{
	PRINTLOG(TEXT("Resolver Ready"));
}

void USceneManager::BuildInitialGuidCache()
{
	UWorld* W = GetWorld();
	if (!W) return;

	for (TActorIterator<AActor> It(W); It; ++It)
	{
		if (UEditSyncComponent* Edit = It->FindComponentByClass<UEditSyncComponent>())
		{
			const FEditMeta& M = Edit->GetMeta();
			if (M.Guid.IsValid())
				GuidCache.FindOrAdd(M.Guid) = *It;
		}
	}
}

void USceneManager::BindWorldDelegates()
{
	UWorld* W = GetWorld();
	if (!W) return;

	OnSpawnedHandle = W->AddOnActorSpawnedHandler(
		FOnActorSpawned::FDelegate::CreateUObject(this, &USceneManager::HandleActorSpawned));

	OnDestroyedHandle = W->AddOnActorDestroyedHandler(
		FOnActorDestroyed::FDelegate::CreateUObject(this, &USceneManager::HandleActorDestroyed));
}

void USceneManager::UnbindWorldDelegates()
{
	if (UWorld* W = GetWorld())
	{
		if (OnSpawnedHandle.IsValid())
			W->RemoveOnActorSpawnedHandler(OnSpawnedHandle);
		if (OnDestroyedHandle.IsValid())
			W->RemoveOnActorDestroyedHandler(OnDestroyedHandle);
	}
}

void USceneManager::HandleActorSpawned(AActor* NewActor)
{
	if (!IsAuthority() || !NewActor) return;
	if (UEditSyncComponent* Edit = NewActor->FindComponentByClass<UEditSyncComponent>())
	{
		const FEditMeta& M = Edit->GetMeta();
		if (M.Guid.IsValid())
			GuidCache.FindOrAdd(M.Guid) = NewActor;
	}
}

void USceneManager::HandleActorDestroyed(AActor* DestroyedActor)
{
	if (!IsAuthority() || !DestroyedActor) return;
	if (UEditSyncComponent* Edit = DestroyedActor->FindComponentByClass<UEditSyncComponent>())
	{
		const FGuid G = Edit->GetMeta().Guid;
		if (G.IsValid()) GuidCache.Remove(G);
	}
}

