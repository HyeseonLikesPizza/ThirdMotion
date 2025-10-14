
#include "UI/WidgetController/LibraryWidgetController.h"
#include "Edit/SceneManager.h"
#include "Edit/AssetResolver.h"
#include "Framework/ThirdMotionPlayerController.h"

void ULibraryWidgetController::Init()
{
	ResolverCached = GetResolver();
	SceneManagerCache = GetSceneManager();
}

void ULibraryWidgetController::QueryByCategory(const FGameplayTag& Category, TArray<const FLibraryRow*>& OutItems)
{
	OutItems.Reset();
	auto* R = GetResolver();
	if (!R || !R->IsReady()) return;
	
	R->GetRowsByCategory(Category, OutItems);
}

void ULibraryWidgetController::BeginPreview(const FGameplayTag& PresetTag)
{
	FTransform Xf = FTransform::Identity;
	DestroyPreviewGhost();
	PreviewGhost = SpawnPreviewGhost_Simple(PresetTag, Xf);
}

void ULibraryWidgetController::UpdatePreviewTransform(const FTransform& Xf)
{
	if (AActor* G = PreviewGhost.Get())
	{
		G->SetActorTransform(Xf, false, nullptr, ETeleportType::TeleportPhysics);
	}
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
	if (!Ghost) return nullptr;
	
	Ghost->SetReplicates(false);
	Ghost->SetActorEnableCollision(false);
	Ghost->SetActorHiddenInGame(false);
	
	return Ghost;
}

void ULibraryWidgetController::DestroyPreviewGhost()
{
	if (AActor* G = PreviewGhost.Get())
	{
		if (IsValid(G)) G->Destroy();
	}
	
	PreviewGhost = nullptr;
	
}


