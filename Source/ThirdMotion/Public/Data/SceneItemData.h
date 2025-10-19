#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "SceneItemData.generated.h"

// Observer 패턴: 데이터 변경 알림 델리게이트
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnSceneItemDataChanged, USceneItemData*, ChangedItem);

/**
 * SceneItemData - Data Model
 * 단일 씬 아이템의 데이터를 저장
 * SceneList에서 관리됨
 */
UCLASS(BlueprintType)
class THIRDMOTION_API USceneItemData : public UObject
{
	GENERATED_BODY()

public:
	// 액터 참조
	UPROPERTY(BlueprintReadOnly, Category = "Scene Item")
	AActor* Actor = nullptr;

	// 표시 이름
	UPROPERTY(BlueprintReadOnly, Category = "Scene Item")
	FString DisplayName;

	// 액터 타입
	UPROPERTY(BlueprintReadOnly, Category = "Scene Item")
	FString ActorType;

	// 자식 아이템들
	UPROPERTY(BlueprintReadOnly, Category = "Scene Item")
	TArray<USceneItemData*> Children;

	// 가시성
	UPROPERTY(BlueprintReadOnly, Category = "Scene Item")
	bool bIsVisible = true;

	// 확장 상태
	UPROPERTY(BlueprintReadOnly, Category = "Scene Item")
	bool bIsExpanded = false;

	// 선택 상태
	UPROPERTY(BlueprintReadOnly, Category = "Scene Item")
	bool bIsSelected = false;

	// Observer 패턴: 데이터 변경 알림
	UPROPERTY(BlueprintAssignable, Category = "Scene Item")
	FOnSceneItemDataChanged OnDataChanged;

	// 액터로부터 데이터 업데이트
	UFUNCTION(BlueprintCallable, Category = "Scene Item")
	void UpdateFromActor();

	// 데이터 변경 알림
	void NotifyDataChanged();
};
