
#pragma once

#include "CoreMinimal.h"
#include "BaseWidgetController.h"
#include "MeshWidgetController.generated.h"


class UAssetResolver;
struct FMeshDataRow;

UCLASS()
class THIRDMOTION_API UMeshWidgetController : public UBaseWidgetController
{
	GENERATED_BODY()

public:
	void Initialize(UAssetResolver* InResolver);
	void AttachView(class UMeshSettingsWidget* InView);
	void SetTargetActor(AActor* InActor);

private:
	UPROPERTY()
	UAssetResolver* Resolver = nullptr;
	
	UPROPERTY()
	UMeshSettingsWidget* View = nullptr;
	
	TWeakObjectPtr<AActor> TargetActor;

	TArray<FMeshDataRow> CachedRows; 

	// View 이벤트 핸들러
	UFUNCTION()
	void HandleMeshPicked(UStaticMesh* NewMesh);

	void RefreshList();
};
