
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

	void InitFromResolver(class UAssetResolver* Resolver);
	void SetTargetActor(AActor* NewTargetActor);
	

private:
	void UpdateSelectionFromActor();

	UFUNCTION()
	void ApplyStaticMesh(UStaticMesh* NewMesh);

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta=(AllowPrivateAccess="true"))
	TWeakObjectPtr<AActor> TargetActor;
};
