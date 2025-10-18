// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "Blueprint/IUserObjectListEntry.h"
#include "SceneItemData.generated.h"

// Observer 패턴: 데이터 변경 알림 델리게이트
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnSceneItemDataChanged, USceneItemData*, ChangedItem);

/**
 * Model - SceneItemData
 * Outliner의 각 액터 정보를 저장하는 데이터 모델
 * Observer 패턴으로 데이터 변경 시 View에 알림
 */
UCLASS(BlueprintType)
class THIRDMOTION_API USceneItemData : public UObject, public IUserObjectListEntry
{
	GENERATED_BODY()

public:
	// Model 데이터
	UPROPERTY(BlueprintReadOnly, Category = "Scene Item")
	AActor* Actor;

	UPROPERTY(BlueprintReadOnly, Category = "Scene Item")
	FString DisplayName;

	UPROPERTY(BlueprintReadOnly, Category = "Scene Item")
	FString ActorType;

	UPROPERTY(BlueprintReadOnly, Category = "Scene Item")
	TArray<USceneItemData*> Children;

	UPROPERTY(BlueprintReadOnly, Category = "Scene Item")
	bool bIsVisible = true;

	UPROPERTY(BlueprintReadOnly, Category = "Scene Item")
	bool bIsExpanded = false;

	// Observer 패턴: 데이터 변경 알림
	UPROPERTY(BlueprintAssignable, Category = "Scene Item")
	FOnSceneItemDataChanged OnDataChanged;

	// Model 업데이트 메서드
	UFUNCTION(BlueprintCallable, Category = "Scene Item")
	void UpdateFromActor();

	// 가시성 토글
	UFUNCTION(BlueprintCallable, Category = "Scene Item")
	void ToggleVisibility();

	// 이름 변경
	UFUNCTION(BlueprintCallable, Category = "Scene Item")
	void SetDisplayName(const FString& NewName);

private:
	// 데이터 변경 알림
	void NotifyDataChanged();
};
