
#pragma once

#include "CoreMinimal.h"
#include "BaseWidgetController.h"
#include "MeshWidgetController.generated.h"


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
	void SetTargetActor(AActor* InActor);

protected:
	virtual void BeginDestroy() override;

private:
	void RefreshList();
	void SyncSelectionToActor();
	void ApplySelectionToCombo(UStaticMesh* Mesh);
	AActor* ResolveSelectionFromArray(const TArray<AActor*>& SelectedActors) const;

	UPROPERTY()
	UAssetResolver* Resolver = nullptr;
	
	UPROPERTY()
	UMeshSettingsWidget* View = nullptr;

	UPROPERTY()
	AThirdMotionPlayerController* PC = nullptr;
	
	TWeakObjectPtr<AActor> TargetActor;

	TArray<FMeshDataRow> CachedRows; 

	// View 이벤트 핸들러
	UFUNCTION()
	void HandleMeshPicked(UStaticMesh* NewMesh);

	UFUNCTION()
	void HandleSelectionChanged(AActor* SelectedActor);

	bool bComboUpdating = false;
};
