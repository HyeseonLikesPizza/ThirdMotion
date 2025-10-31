
#include "UI/WidgetController/LibraryWidgetController.h"

#include "Data/LibraryItemObject.h"
#include "Edit/SceneManager.h"
#include "Edit/AssetResolver.h"
#include "Framework/ThirdMotionPlayerController.h"
#include "Data/SceneList.h"

void ULibraryWidgetController::Init()
{
	ResolverCached = GetResolver();
	SceneManagerCache = GetSceneManager();
}

void ULibraryWidgetController::QueryByCategory(const FGameplayTag& Category, TArray<ULibraryItemObject*>& OutItems)
{
	OutItems.Reset();
	auto* R = GetResolver();
	if (!R || !R->IsReady()) return;

	TArray<const FLibraryRow*> RowArr;
	R->GetRowsByCategory(Category, RowArr);

	for (auto* It : RowArr)
	{
		ULibraryItemObject* Obj = NewObject<ULibraryItemObject>(this);
		Obj->DisplayName = FText::FromName(It->DisplayName);
		Obj->Icon = It->Icon;
		Obj->Tag = It->PresetTag;
		Obj->Type = It->IconType;
		Obj->NextCategoryTag = It->NextCategory;

		OutItems.Add(Obj);
	}
}

void ULibraryWidgetController::GetDirectChildrenCategories(const FGameplayTag& Category,
	TArray<ULibraryItemObject*>& OutItems)
{
	OutItems.Reset();
	auto* R = GetResolver();
	if (!R || !R->IsReady()) return;

	TArray<FGameplayTag> Categories;
	R->GetDirectChildrenCategories(Category, Categories);

	for (auto category : Categories)
	{
		ULibraryItemObject* Obj = NewObject<ULibraryItemObject>(this);
		const FLibraryRow* Row =  R->GetRowByCategory(category);
		Obj->DisplayName = FText::FromName(Row->DisplayName);
		Obj->Icon = Row->Icon;
		Obj->Tag = Row->PresetTag;
		Obj->Type = Row->IconType;
		Obj->NextCategoryTag = Row->NextCategory;

		OutItems.Add(Obj);
	}
}

void ULibraryWidgetController::BeginPreview(const FGameplayTag& PresetTag)
{
	FTransform Xf = FTransform::Identity;
	DestroyPreviewGhost();
	PreviewGhost = SpawnPreviewGhost_Simple(PresetTag, Xf);
}

void ULibraryWidgetController::UpdatePreviewTransform(const FTransform& Xf)
{
	if (!PreviewGhost.IsValid()) return;
	PreviewGhost->SetActorTransform(Xf, false, nullptr, ETeleportType::TeleportPhysics);
}

void ULibraryWidgetController::CancelPreview()
{
	DestroyPreviewGhost();
}

void ULibraryWidgetController::CommitSpawn(const FGameplayTag& PresetTag, const FTransform& FinalTransform)
{
	if (APlayerController* PC = GetWorld()->GetFirstPlayerController())
	{
		if (AThirdMotionPlayerController* MyPC = Cast<AThirdMotionPlayerController>(PC))
		{
			MyPC->Server_RequestSpawnByTag(PresetTag, FinalTransform);
		}
	}

	DestroyPreviewGhost();
}

UAssetResolver* ULibraryWidgetController::GetResolver() const
{
	return GetWorld() ? GetWorld()->GetSubsystem<UAssetResolver>() : nullptr;
}

USceneManager* ULibraryWidgetController::GetSceneManager() const
{
	return GetWorld() ? GetWorld()->GetSubsystem<USceneManager>() : nullptr;
}

AActor* ULibraryWidgetController::SpawnPreviewGhost_Simple(const FGameplayTag& PresetTag, const FTransform& Xf)
{
	UAssetResolver* R = GetWorld()->GetSubsystem<UAssetResolver>();
	if (!R) return nullptr;

	const FLibraryRow* Row = R->FindRowByTag(PresetTag);

	UClass* PreviewClass = Row->ClassRef.LoadSynchronous();

	AActor* Ghost = GetWorld()->SpawnActor<AActor>(PreviewClass, Xf);
	UE_LOG(LogTemp, Warning, TEXT("PreviewClass: %s"), *PreviewClass->GetName());
	if (!Ghost) return nullptr;

	Ghost->SetReplicates(false);
	Ghost->SetActorEnableCollision(false);
	Ghost->SetActorHiddenInGame(false);

	// Ghost Actor에 DisplayName 설정 (Ghost 표시)
	FString GhostName = FString::Printf(TEXT("%s (Ghost)"), *Row->DisplayName.ToString());
	Ghost->SetActorLabel(GhostName);

	UE_LOG(LogTemp, Warning, TEXT("LibraryWidgetController: Ghost spawned - %s, Class=%s"),
		*GhostName, *Ghost->GetClass()->GetName());

	// SceneManager를 통해 SceneList에 Ghost 추가
	/*if (USceneManager* SceneMgr = GetWorld()->GetSubsystem<USceneManager>())
	{
		UE_LOG(LogTemp, Warning, TEXT("SceneMgr: %s"), *SceneMgr->GetName());
		if (USceneList* SceneListRef = SceneMgr->GetSceneList())
		{
			UE_LOG(LogTemp, Warning, TEXT("SceneListRef: %s"), *SceneListRef->GetName());
			UE_LOG(LogTemp, Warning, TEXT("LibraryWidgetController: Adding Ghost to SceneList"));
			SceneListRef->AddActor(Ghost);
		}
		else
		{
			UE_LOG(LogTemp, Error, TEXT("LibraryWidgetController: SceneList is NULL!"));
		}
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("LibraryWidgetController: SceneManager is NULL!"));
	}*/

	return Ghost;
}

void ULibraryWidgetController::DestroyPreviewGhost()
{
	if (AActor* G = PreviewGhost.Get())
	{
		if (IsValid(G))
		{
			// SceneList에서 Ghost 제거
			if (USceneManager* SceneMgr = GetWorld()->GetSubsystem<USceneManager>())
			{
				if (USceneList* SceneListRef = SceneMgr->GetSceneList())
				{
					SceneListRef->RemoveActor(G);
				}
			}

			G->Destroy();
		}
	}

	PreviewGhost = nullptr;
}


