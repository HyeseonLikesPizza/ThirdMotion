#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "ActorDetailsData.generated.h"

// Observer 패턴: 데이터 변경 알림 델리게이트
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnActorDetailsChanged, UActorDetailsData*, ChangedData);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(FOnMeshChanged, UMeshComponent*, Component, UObject*, NewMesh, UObject*, OldMesh);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_FourParams(FOnMaterialChanged, UMeshComponent*, Component, int32, MaterialIndex, UMaterialInterface*, NewMaterial, UMaterialInterface*, OldMaterial);

/**
 * ActorPropertyData - 단일 속성 데이터
 * Mesh 또는 Material 속성 정보를 저장
 */
USTRUCT(BlueprintType)
struct FActorPropertyData
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadWrite, Category = "Property")
	FString PropertyName;

	UPROPERTY(BlueprintReadWrite, Category = "Property")
	UObject* PropertyValue;

	UPROPERTY(BlueprintReadWrite, Category = "Property")
	UClass* PropertyClass;

	// Mesh Component 참조
	UPROPERTY(BlueprintReadWrite, Category = "Property")
	UMeshComponent* TargetComponent;

	// Material인 경우 Material Index
	UPROPERTY(BlueprintReadWrite, Category = "Property")
	int32 MaterialIndex;

	UPROPERTY(BlueprintReadWrite, Category = "Property")
	bool bIsMaterialProperty;

	FActorPropertyData()
		: PropertyName(TEXT(""))
		, PropertyValue(nullptr)
		, PropertyClass(nullptr)
		, TargetComponent(nullptr)
		, MaterialIndex(0)
		, bIsMaterialProperty(false)
	{
	}
};

/**
 * ActorDetailsData - Data Model (MVC Pattern)
 * 선택된 액터의 Mesh와 Material 속성 데이터를 관리
 *
 * 책임:
 * - 액터의 속성 데이터 저장
 * - 속성 변경 시 Observer에게 알림
 * - 데이터 무결성 유지
 */
UCLASS(BlueprintType)
class THIRDMOTION_API UActorDetailsData : public UObject
{
	GENERATED_BODY()

public:
	// 선택된 액터
	UPROPERTY(BlueprintReadOnly, Category = "Actor Details")
	AActor* SelectedActor;

	// 액터 이름
	UPROPERTY(BlueprintReadOnly, Category = "Actor Details")
	FString ActorName;

	// 액터 클래스 이름
	UPROPERTY(BlueprintReadOnly, Category = "Actor Details")
	FString ActorClassName;

	// 모든 속성 목록 (Mesh + Materials)
	UPROPERTY(BlueprintReadOnly, Category = "Actor Details")
	TArray<FActorPropertyData> Properties;

	// Observer 패턴: 데이터 변경 알림
	UPROPERTY(BlueprintAssignable, Category = "Actor Details")
	FOnActorDetailsChanged OnDataChanged;

	UPROPERTY(BlueprintAssignable, Category = "Actor Details")
	FOnMeshChanged OnMeshChanged;

	UPROPERTY(BlueprintAssignable, Category = "Actor Details")
	FOnMaterialChanged OnMaterialChanged;

	// 액터로부터 데이터 업데이트
	UFUNCTION(BlueprintCallable, Category = "Actor Details")
	void UpdateFromActor(AActor* InActor);

	// 속성 추가
	void AddProperty(const FActorPropertyData& Property);

	// 속성 클리어
	void ClearProperties();

	// Mesh 변경
	void SetMesh(UMeshComponent* Component, UObject* NewMesh);

	// Material 변경
	void SetMaterial(UMeshComponent* Component, int32 MaterialIndex, UMaterialInterface* NewMaterial);

	// 데이터 변경 알림
	void NotifyDataChanged();

private:
	// 액터의 Mesh Component들 수집
	void CollectMeshComponents();

	// StaticMeshComponent 속성 추가
	void AddStaticMeshProperty(class UStaticMeshComponent* MeshComponent);

	// SkeletalMeshComponent 속성 추가
	void AddSkeletalMeshProperty(class USkeletalMeshComponent* SkeletalMeshComponent);

	// Material 속성들 추가
	void AddMaterialProperties(UMeshComponent* MeshComponent);
};
