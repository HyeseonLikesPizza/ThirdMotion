#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "ActorDetailsController.generated.h"

class AActor;
class UActorDetailsData;
class UMeshComponent;
class UMaterialInterface;

/**
 * ActorDetailsController - Controller 역할 (MVC Pattern)
 *
 * 책임:
 * - ActorDetails 관련 모든 비즈니스 로직 처리
 * - Model(ActorDetailsData) 업데이트 처리
 * - 사용자 액션(Mesh 변경, Material 변경)을 받아 처리
 * - AssetPicker 선택 처리
 *
 * 역할 분리:
 * - Controller: 로직 처리 및 데이터 조작
 * - Model: ActorDetailsData를 통해 데이터 관리
 * - View: ActorDetailsPanel에게 UI 업데이트 위임
 */
UCLASS(BlueprintType)
class THIRDMOTION_API UActorDetailsController : public UObject
{
	GENERATED_BODY()

public:
	// 초기화
	void Initialize(UWorld* InWorld, UActorDetailsData* InDetailsData = nullptr);

	// ActorDetailsData 설정
	UFUNCTION(BlueprintCallable, Category = "Actor Details Controller")
	void SetDetailsData(UActorDetailsData* InDetailsData);

	UFUNCTION(BlueprintCallable, Category = "Actor Details Controller")
	UActorDetailsData* GetDetailsData() const { return DetailsData; }

	// 액터 선택
	UFUNCTION(BlueprintCallable, Category = "Actor Details Controller")
	void SelectActor(AActor* InActor);

	// 액터 선택 해제
	UFUNCTION(BlueprintCallable, Category = "Actor Details Controller")
	void ClearSelection();

	// Mesh 변경
	UFUNCTION(BlueprintCallable, Category = "Actor Details Controller")
	void ChangeMesh(UMeshComponent* Component, UObject* NewMesh);

	// Material 변경
	UFUNCTION(BlueprintCallable, Category = "Actor Details Controller")
	void ChangeMaterial(UMeshComponent* Component, int32 MaterialIndex, UMaterialInterface* NewMaterial);

	// AssetPicker에서 에셋 선택 처리
	UFUNCTION(BlueprintCallable, Category = "Actor Details Controller")
	void OnAssetSelected(UObject* SelectedAsset, UMeshComponent* TargetComponent, bool bIsMaterial, int32 MaterialIndex);

	// Observer 패턴: 액터 선택 변경 알림
	DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnActorSelected, AActor*, SelectedActor);
	UPROPERTY(BlueprintAssignable, Category = "Actor Details Controller")
	FOnActorSelected OnActorSelected;

	// Observer 패턴: 속성 변경 알림
	DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnPropertyChanged);
	UPROPERTY(BlueprintAssignable, Category = "Actor Details Controller")
	FOnPropertyChanged OnPropertyChanged;

private:
	UPROPERTY()
	UWorld* World;

	// ActorDetailsData 참조 (Model)
	UPROPERTY()
	UActorDetailsData* DetailsData;

	// 알림 함수
	void NotifyActorSelected(AActor* Actor);
	void NotifyPropertyChanged();
};
