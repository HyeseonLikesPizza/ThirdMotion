
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
	static TSoftObjectPtr<UStaticMesh> PreviewCube(TEXT("/Engine/EditorMeshes/EditorCube.EditorCube"));
	UStaticMesh* Mesh = PreviewCube.LoadSynchronous();

	AActor* Ghost = GetWorld()->SpawnActor<AActor>(AActor::StaticClass(), Xf);
	Ghost->SetReplicates(false);
	Ghost->SetActorEnableCollision(false);
	Ghost->SetActorHiddenInGame(false);

	UStaticMeshComponent* SMC = NewObject<UStaticMeshComponent>(Ghost);
	SMC->SetStaticMesh(Mesh);
	


	
	return nullptr;
}

void ULibraryWidgetController::DestroyPreviewGhost()
{
}
