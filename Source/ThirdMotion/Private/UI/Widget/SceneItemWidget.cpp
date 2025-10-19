#include "UI/Widget/SceneItemWidget.h"
#include "Data/SceneItemData.h"
#include "UI/WidgetController/SceneController.h"
#include "Components/TextBlock.h"
#include "Components/Button.h"
#include "GameFramework/Actor.h"

void USceneItemWidget::SetSceneController(USceneController* InController)
{
	SceneController = InController;
}

void USceneItemWidget::NativeConstruct()
{
	Super::NativeConstruct();

	if (VisibilityButton)
	{
		VisibilityButton->OnClicked.AddDynamic(this, &USceneItemWidget::OnVisibilityButtonClicked);
	}
}

void USceneItemWidget::NativeDestruct()
{
	if (ItemData)
	{
		ItemData->OnDataChanged.RemoveDynamic(this, &USceneItemWidget::OnModelDataChanged);
	}

	Super::NativeDestruct();
}

void USceneItemWidget::NativeOnListItemObjectSet(UObject* ListItemObject)
{
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
		// Model 변경 감지 등록
		ItemData->OnDataChanged.AddDynamic(this, &USceneItemWidget::OnModelDataChanged);

		// View 업데이트
		UpdateView();

	}
}

void USceneItemWidget::OnModelDataChanged(USceneItemData* ChangedData)
{
	if (ChangedData == ItemData)
	{
		UpdateView();
	}
}

void USceneItemWidget::UpdateView()
{
	if (!ItemData) return;

	// 선택 상태에 따른 색상 결정 (빨간색으로 변경)
	FLinearColor TextColor = ItemData->bIsSelected ? FLinearColor::Red : FLinearColor::White;

	// 이름 표시
	if (ItemLabel)
	{
		ItemLabel->SetText(FText::FromString(ItemData->DisplayName));
		ItemLabel->SetColorAndOpacity(TextColor);
	}

	// 타입 표시
	if (ItemType)
	{
		FString TypeText = ItemData->ActorType.IsEmpty() ? TEXT("Actor") : ItemData->ActorType;
		ItemType->SetText(FText::FromString(TypeText));

		// 타입은 선택 시 빨간색, 미선택 시 회색
		FLinearColor TypeColor = ItemData->bIsSelected ?
			FLinearColor::Red : // 빨간색
			FLinearColor(0.5f, 0.5f, 0.5f, 1.0f);  // 회색
		ItemType->SetColorAndOpacity(TypeColor);
	}

	// 가시성 상태 표시
	if (VisibilityButton)
	{
		FLinearColor ButtonColor = ItemData->bIsVisible ? FLinearColor::White : FLinearColor::Gray;
		VisibilityButton->SetColorAndOpacity(ButtonColor);
	}
}

void USceneItemWidget::OnVisibilityButtonClicked()
{
	// SceneController를 통해 가시성 토글
	if (SceneController && ItemData && ItemData->Actor)
	{
		SceneController->ToggleActorVisibility(ItemData->Actor);
	}
}

void USceneItemWidget::OnItemClicked()
{
	// SceneController를 통해 액터 선택
	if (SceneController && ItemData && ItemData->Actor)
	{
		SceneController->SelectActor(ItemData->Actor);
	}
}
