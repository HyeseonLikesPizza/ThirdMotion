
#pragma once

#include "CoreMinimal.h"
#include "BaseWidgetController.h"
#include "MeshWidgetController.generated.h"


class UMaterialGeneratePanel;
struct FMaterialEntryRow;
class AThirdMotionPlayerController;
class UAssetResolver;
class UMeshSettingsWidget;
class USceneController;
class UStaticMesh;
class AActor;
struct FMeshDataRow;

UCLASS()
class THIRDMOTION_API UMeshWidgetController : public UBaseWidgetController
{
	GENERATED_BODY()

public:
	void Initialize(UAssetResolver* InResolver);
	void AttachView(UMeshSettingsWidget* InView);
	void BindPlayerController(AThirdMotionPlayerController* InPC);
	void BindMaterialPanel(UMaterialGeneratePanel* InView);
	void SetTargetActor(AActor* InActor);

protected:
	virtual void BeginDestroy() override;

private:
	void RefreshList();
	void SyncSelectionToActor();
	void ApplySelectionToMeshCombo(UStaticMesh* Mesh);
	void ApplySelectionToMaterialCombo(UMaterialInterface* Material);
	AActor* ResolveSelectionFromArray(const TArray<AActor*>& SelectedActors) const;

	UPROPERTY()
	UAssetResolver* Resolver = nullptr;
	
	UPROPERTY()
	UMeshSettingsWidget* MeshPanel = nullptr;

	UPROPERTY()
	UMaterialGeneratePanel* MaterialPanel = nullptr;

	UPROPERTY()
	AThirdMotionPlayerController* PC = nullptr;
	
	TWeakObjectPtr<AActor> TargetActor;

	TArray<FMeshDataRow> CachedMeshRows;

	TArray<FMaterialEntryRow> CachedMaterialRows;

	// View 이벤트 핸들러
	UFUNCTION()
	void HandleMeshPicked(UStaticMesh* NewMesh);

	UFUNCTION()
	void HandleMaterialPicked(UMaterialInterface* NewMaterial);

	UFUNCTION()
	void HandleSelectionChanged(AActor* SelectedActor);

	UFUNCTION()
	void HandleMaterialCreated(UMaterialPreviewData* InObject);

	bool bComboUpdating = false;
};
