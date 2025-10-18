// Fill out your copyright notice in the Description page of Project Settings.

#include "UI/Widget/SceneItemWidget.h"
#include "UI/WidgetController/SceneItemData.h"
#include "Components/TextBlock.h"
#include "Components/Button.h"
#include "Components/Image.h"

void USceneItemWidget::NativeConstruct()
{
	Super::NativeConstruct();

	// Outliner 기능: 가시성 버튼 이벤트 바인딩
	if (VisibilityButton)
	{
		VisibilityButton->OnClicked.AddDynamic(this, &USceneItemWidget::OnVisibilityButtonClicked);
	}
}

void USceneItemWidget::NativeDestruct()
{
	// Observer 패턴: Model 감시 해제
	if (ItemData)
	{
		ItemData->OnDataChanged.RemoveDynamic(this, &USceneItemWidget::OnModelDataChanged);
	}

	Super::NativeDestruct();
}

/**
 * Observer 패턴 + MVC: Model 설정 시 호출
 * TreeView/ListView가 자동으로 호출
 */
void USceneItemWidget::NativeOnListItemObjectSet(UObject* ListItemObject)
{
	// IUserObjectListEntry 인터페이스 메서드 구현
	IUserObjectListEntry::NativeOnListItemObjectSet(ListItemObject);

	// 기존 Observer 해제
	if (ItemData)
	{
		ItemData->OnDataChanged.RemoveDynamic(this, &USceneItemWidget::OnModelDataChanged);
	}

	// Model 설정
	ItemData = Cast<USceneItemData>(ListItemObject);

	if (ItemData)
	{
		// Observer 패턴: Model 변경 감지 등록
		ItemData->OnDataChanged.AddDynamic(this, &USceneItemWidget::OnModelDataChanged);

		// View 업데이트
		UpdateView();

		UE_LOG(LogTemp, Log, TEXT("SceneItemWidget: Bound to item '%s' (Type: '%s')"), *ItemData->DisplayName, *ItemData->ActorType);
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("SceneItemWidget: Failed to cast ListItemObject to USceneItemData"));
	}
}

/**
 * Observer 패턴: Model이 변경되면 자동으로 호출됨
 */
void USceneItemWidget::OnModelDataChanged(USceneItemData* ChangedData)
{
	if (ChangedData == ItemData)
	{
		UpdateView();
		UE_LOG(LogTemp, Log, TEXT("SceneItemWidget: Model changed, updating view for '%s'"), *ItemData->DisplayName);
	}
}

/**
 * View 업데이트 (Model -> View)
 */
void USceneItemWidget::UpdateView()
{
	if (!ItemData)
	{
		return;
	}

	// 이름 표시
	if (ItemLabel)
	{
		ItemLabel->SetText(FText::FromString(ItemData->DisplayName));
	}

	// 타입 표시
	if (ItemType)
	{
		FString TypeText = ItemData->ActorType.IsEmpty() ? TEXT("Actor") : ItemData->ActorType;
		ItemType->SetText(FText::FromString(TypeText));
	}

	// Outliner 기능: 가시성 상태 표시
	if (VisibilityButton)
	{
		// 가시성에 따라 버튼 색상 변경 (예시)
		FLinearColor ButtonColor = ItemData->bIsVisible ? FLinearColor::White : FLinearColor::Gray;
		// VisibilityButton->SetColorAndOpacity(ButtonColor);
	}
}

/**
 * Outliner 기능: 가시성 토글
 */
void USceneItemWidget::OnVisibilityButtonClicked()
{
	if (ItemData)
	{
		// Model 변경 (Controller를 통하지 않고 직접 변경 - MVP 패턴 방식도 가능)
		ItemData->ToggleVisibility();
		// Observer 패턴에 의해 OnModelDataChanged가 자동 호출되어 View 업데이트됨

		UE_LOG(LogTemp, Log, TEXT("SceneItemWidget: Toggled visibility for '%s' to %s"),
			*ItemData->DisplayName, ItemData->bIsVisible ? TEXT("Visible") : TEXT("Hidden"));
	}
}

