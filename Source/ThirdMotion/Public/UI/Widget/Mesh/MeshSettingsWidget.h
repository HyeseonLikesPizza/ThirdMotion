
#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "MeshSettingsWidget.generated.h"

UCLASS()
class THIRDMOTION_API UMeshSettingsWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	virtual void NativeConstruct() override;
	
	UPROPERTY(meta=(BindWidget))
	class UMeshListCombo* MeshListCombo;

	UPROPERTY(meta=(BindWidget))
	class UMaterialListCombo* MaterialListCombo;

	void InitFromResolver(class UAssetResolver* Resolver);
	void SetTargetActor(AActor* NewTargetActor);
	

private:
	void UpdateSelectionFromActor();

	UFUNCTION()
	void ApplyStaticMesh(UStaticMesh* NewMesh);

	UFUNCTION()
	void ApplyMaterial(UMaterialInterface* NewMaterial);

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta=(AllowPrivateAccess="true"))
	TWeakObjectPtr<AActor> TargetActor;
};
