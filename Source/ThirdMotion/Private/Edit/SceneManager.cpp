
#include "Edit/SceneManager.h"

#include "EngineUtils.h"
#include "GameplayTagContainer.h"
#include "Edit/AssetResolver.h"
#include "Edit/EditSyncComponent.h"
#include "Edit/LightEditLibrary.h"
#include "Data/SceneList.h"
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

	// 서버와 클라이언트 모두 델리게이트 바인드
	BuildInitialGuidCache();
	BindWorldDelegates();
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

	// 서버가 고유 이름 생성 (타입별 카운터 사용)
	FString UniqueActorName = GenerateUniqueActorName(ClassToSpawn->GetName());

	AttachEditComponentAndMeta(NewActor, *Row);

	/*
	// EditMeta에 고유 Actor 이름 저장 (네트워크로 복제됨)
	if (UEditSyncComponent* Edit = NewActor->FindComponentByClass<UEditSyncComponent>())
	{
		FEditMeta Meta = Edit->GetMeta();
		Meta.DisplayName = FName(*UniqueActorName);
		Edit->SetMeta_Unsafe(Meta);
	}
	*/

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
		FPropertyDelta Delta;
		Delta.Op = EPropOp::SetTransform;
		Delta.TransformParam = T;
		
		if (UEditSyncComponent* Edit = Actor->FindComponentByClass<UEditSyncComponent>())
		{
			Edit->ServerApplyPropertyDelta_Internal(Delta);
			return true;
		}
	}
	return false;
}

bool USceneManager::ApplyPropertyDelta(const FGuid& Guid, const FPropertyDelta& Delta)
{
	check(IsAuthority());

	AActor* Actor = FindByGuid(Guid);
	if (!Actor) return false;

	// Light 속성: LightEditLibrary로 직접 처리
	const FName Key = Delta.PropertyTag.GetTagName();
	if (Key.ToString().StartsWith("Property.Light."))
	{
		bool bSuccess = ULightEditLibrary::ApplyLightPropertyDelta(Actor, Delta);
		if (bSuccess)
		{
			// 히스토리에 추가하여 복제
			if (UEditSyncComponent* Edit = Actor->FindComponentByClass<UEditSyncComponent>())
			{
				Edit->ServerApplyPropertyDelta_Internal(Delta);
			}
		}
		return bSuccess;
	}

	// 다른 속성: EditSyncComponent를 통해 처리
	if (UEditSyncComponent* Edit = Actor->FindComponentByClass<UEditSyncComponent>())
	{
		Edit->ServerApplyPropertyDelta_Internal(Delta);
		return true;
	}

	return false;
}

bool USceneManager::ToggleActorVisibility(const FGuid& Guid)
{
	check(IsAuthority());

	AActor* Actor = FindByGuid(Guid);
	if (!Actor) return false;

	// 현재 가시성 상태 가져오기
	bool bCurrentlyHidden = Actor->IsHidden();
	bool bNewVisibility = bCurrentlyHidden; // 숨겨져 있으면 true (보이게)

	// 가시성 토글
	Actor->SetActorHiddenInGame(bCurrentlyHidden);

#if WITH_EDITOR
	Actor->SetIsTemporarilyHiddenInEditor(bCurrentlyHidden);
#endif

	UE_LOG(LogTemp, Log, TEXT("SceneManager: Toggled visibility for Actor %s - NewHidden=%d"),
		*Actor->GetName(), bCurrentlyHidden);

	return true;
}

void USceneManager::AttachEditComponentAndMeta(AActor* Actor, const struct FLibraryRow& Row)
{
	auto* Edit = NewObject<UEditSyncComponent>(Actor);
	Edit->RegisterComponent();
	Edit->InitMetaFromPreset(Row);
}

FString USceneManager::GenerateUniqueActorName(const FString& BaseClassName)
{
	// BP_Chair_C → BP_Chair 로 변환
	FString CleanName = BaseClassName;
	if (CleanName.EndsWith(TEXT("_C")))
	{
		CleanName.LeftChopInline(2);
	}

	// 해당 타입의 카운터 가져오기/증가
	int32& Counter = ActorTypeCounters.FindOrAdd(CleanName, 0);
	Counter++;

	// "BP_Chair1" 형식으로 생성
	FString UniqueName = FString::Printf(TEXT("%s%d"), *CleanName, Counter);

	return UniqueName;
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

void USceneManager::RegisterSceneList(USceneList* InSceneList)
{
	SceneListRef = InSceneList;
}

void USceneManager::HandleActorSpawned(AActor* NewActor)
{
	if (!NewActor) return;

	// GuidCache 업데이트 (서버와 클라이언트 모두)
	if (UEditSyncComponent* Edit = NewActor->FindComponentByClass<UEditSyncComponent>())
	{
		const FEditMeta& M = Edit->GetMeta();
		if (M.Guid.IsValid())
			GuidCache.FindOrAdd(M.Guid) = NewActor;
	}

	// SceneList 자동 업데이트 (서버와 클라이언트 모두)
	if (SceneListRef.IsValid())
	{
		SceneListRef->AddActor(NewActor);
	}
}

void USceneManager::HandleActorDestroyed(AActor* DestroyedActor)
{
	if (!DestroyedActor) return;

	// GuidCache 업데이트 (서버와 클라이언트 모두)
	if (UEditSyncComponent* Edit = DestroyedActor->FindComponentByClass<UEditSyncComponent>())
	{
		const FGuid G = Edit->GetMeta().Guid;
		if (G.IsValid()) GuidCache.Remove(G);
	}

	// SceneList 자동 업데이트 (서버와 클라이언트 모두)
	if (SceneListRef.IsValid())
	{
		SceneListRef->RemoveActor(DestroyedActor);
	}
}

