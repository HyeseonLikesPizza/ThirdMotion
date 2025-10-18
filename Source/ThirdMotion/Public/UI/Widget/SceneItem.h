// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Blueprint/IUserObjectListEntry.h"
#include "SceneItemWidget.generated.h"

class UTextBlock;
class USceneItemData;
class UButton;
class UImage;

/**
 * View - SceneItemWidget
 * MVC 패턴의 View 역할
 * Observer 패턴으로 Model 변경사항을 감지하여 UI 업데이트
 * Outliner의 각 Row를 표시
 */
UCLASS()
class THIRDMOTION_API USceneItemWidget : public UUserWidget, public IUserObjectListEntry
{
	GENERATED_BODY()

public:
	// IUserObjectListEntry 인터페이스 구현
	virtual void NativeOnListItemObjectSet(UObject* ListItemObject) override;
	virtual void NativeConstruct() override;
	virtual void NativeDestruct() override;

	// 바인딩할 UI 요소들 (Outliner처럼)
	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	UTextBlock* ItemLabel;

	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	UTextBlock* ItemType;

	// Outliner 기능: 가시성 토글 버튼
	UPROPERTY(BlueprintReadWrite, meta = (BindWidgetOptional))
	UButton* VisibilityButton;

	// Outliner 기능: 아이콘 이미지
	UPROPERTY(BlueprintReadWrite, meta = (BindWidgetOptional))
	UImage* ActorIcon;

protected:
	// 현재 표시 중인 데이터 (Model)
	UPROPERTY(BlueprintReadOnly, Category = "Scene Item")
	USceneItemData* ItemData;

	// Observer 패턴: Model 변경 감지
	UFUNCTION()
	void OnModelDataChanged(USceneItemData* ChangedData);

	// Outliner 기능: 가시성 토글
	UFUNCTION()
	void OnVisibilityButtonClicked();

	// UI 업데이트
	void UpdateView();
};
