#include "UI/Panel/RightPanel.h"
#include "UI/Widget/SceneItemWidget.h"
#include "UI/Widget/SceneListWidget.h"
#include "UI/WidgetController/RightPanelController.h"
#include "UI/WidgetController/SceneController.h"
#include "Components/TextBlock.h"
#include "Components/WidgetSwitcher.h"
#include "Engine/World.h"
#include "Framework/Application/SlateApplication.h"
#include "Engine/GameInstance.h"


//if WITH_EDITOR: 에디터 전용 코드를 구분하는 전처리 지시자
#if WITH_EDITOR
// 에디터에서 실행 
#include "Engine/Selection.h"
#include "Editor.h"
#endif

/**
 * Row Widget 생성 - View만 담당
 */

void URightPanel::NativeConstruct()
{
	Super::NativeConstruct();

	// RightPanelController 초기화
	InitializeRightPanelController();

	UE_LOG(LogTemp, Log, TEXT("RightPanel: NativeConstruct completed"));
}

void URightPanel::InitializeRightPanelController()
{
	if (!RightPanelController)
	{
		RightPanelController = NewObject<URightPanelController>(this);
		RightPanelController->InitializeWithRightPanel(this);

		UE_LOG(LogTemp, Log, TEXT("RightPanel: RightPanelController initialized"));
	}
}


void URightPanel::SetWidgetSwitcherIndex(int32 Index)
{
	if (!WidgetSwitcher_Right)
	{
		UE_LOG(LogTemp, Warning, TEXT("RightPanel View: WidgetSwitcher_Right is null"));
		return;
	}

	// WidgetSwitcher 인덱스 변경 (View 기능만)
	WidgetSwitcher_Right->SetActiveWidgetIndex(Index);

	UE_LOG(LogTemp, Log, TEXT("RightPanel View: WidgetSwitcher set to index %d"), Index);
}

void URightPanel::SetRightPanelVisibility(bool bVisible)
{
	if (bVisible)
	{
		SetVisibility(ESlateVisibility::Visible);
		UE_LOG(LogTemp, Log, TEXT("RightPanel View: Panel shown"));
	}
	else
	{
		SetVisibility(ESlateVisibility::Collapsed);
		UE_LOG(LogTemp, Log, TEXT("RightPanel View: Panel hidden"));
	}
}

USceneController* URightPanel::GetSceneController() const
{
	if (SceneListWidget)
	{
		return SceneListWidget->GetSceneController();
	}
	return nullptr;
}
