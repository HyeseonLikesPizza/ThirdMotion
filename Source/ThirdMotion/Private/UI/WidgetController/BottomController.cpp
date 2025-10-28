// Fill out your copyright notice in the Description page of Project Settings.

#include "UI/WidgetController/BottomController.h"
#include "UI/Panel/RightPanel.h"
#include "UI/Panel/BottomView.h"
#include "UI/WidgetController/RightPanelController.h"

void UBottomController::InitializeWithRightPanel(URightPanel* InRightPanel)
{
	RightPanel = InRightPanel;
	CurrentPanelType = ERightPanelType::Scene; // 기본값: Scene

	UE_LOG(LogTemp, Log, TEXT("BottomController: Initialized with RightPanel"));
}

void UBottomController::InitializeWithBottomView(UBottomView* InBottomView)
{
	BottomView = InBottomView;
	CurrentViewType = EBottomViewType::None; // 기본값: None (숨김)

	// 초기 상태: BottomView 숨김
	if (BottomView)
	{
		BottomView->SetBottomViewVisibility(false);
	}

	UE_LOG(LogTemp, Log, TEXT("BottomController: Initialized with BottomView"));
}

void UBottomController::ToggleScenePanel()
{
	TogglePanel(ERightPanelType::Scene);
}

void UBottomController::TogglePropertiesPanel()
{
	TogglePanel(ERightPanelType::Properties);
}

void UBottomController::ToggleXYZPanel()
{
	TogglePanel(ERightPanelType::XYZ);
}

void UBottomController::ToggleUserListPanel()
{
	TogglePanel(ERightPanelType::UserList);
}

void UBottomController::ToggleMaterialView()
{
	ToggleView(EBottomViewType::Material);
}

void UBottomController::ToggleMemoView()
{
	ToggleView(EBottomViewType::Memo);
}


void UBottomController::TogglePanel(ERightPanelType PanelType)
{
	if (!RightPanel)
	{
		UE_LOG(LogTemp, Warning, TEXT("BottomController: RightPanel is null"));
		return;
	}

	// RightPanelController를 통해 패널 토글
	if (URightPanelController* RightPanelController = RightPanel->GetRightPanelController())
	{
		RightPanelController->TogglePanel(PanelType);

		// 패널이 보이는지 확인하여 CurrentPanelType 업데이트
		if (RightPanelController->IsPanelVisible())
		{
			CurrentPanelType = PanelType;
			OnRightPanelChanged.Broadcast(PanelType);
			UE_LOG(LogTemp, Log, TEXT("BottomController: Toggled panel %d (shown)"), static_cast<int32>(PanelType));
		}
		else
		{
			UE_LOG(LogTemp, Log, TEXT("BottomController: Toggled panel %d (hidden)"), static_cast<int32>(PanelType));
		}
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("BottomController: RightPanelController is null"));
	}
}

void UBottomController::ToggleView(EBottomViewType ViewType)
{
	if (!BottomView)
	{
		UE_LOG(LogTemp, Warning, TEXT("BottomController: BottomView is null"));
		return;
	}

	// 같은 뷰를 다시 클릭하면 숨김
	if (CurrentViewType == ViewType)
	{
		// 뷰 숨김
		BottomView->SetBottomViewVisibility(false);
		CurrentViewType = EBottomViewType::None;
		OnBottomViewChanged.Broadcast(EBottomViewType::None);
		UE_LOG(LogTemp, Log, TEXT("BottomController: BottomView hidden"));
	}
	else
	{
		// 뷰 전환
		int32 Index = 0;
		switch (ViewType)
		{
		case EBottomViewType::Material:
			Index = 0; // Material은 WidgetSwitcher 인덱스 0
			break;
		case EBottomViewType::Memo:
			Index = 1; // Memo는 WidgetSwitcher 인덱스 1
			break;
		default:
			break;
		}

		BottomView->SetBottomViewSwitcherIndex(Index);
		BottomView->SetBottomViewVisibility(true);
		CurrentViewType = ViewType;
		OnBottomViewChanged.Broadcast(ViewType);
		UE_LOG(LogTemp, Log, TEXT("BottomController: BottomView switched to %d"), static_cast<int32>(ViewType));
	}
}
