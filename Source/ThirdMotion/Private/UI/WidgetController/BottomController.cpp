// Fill out your copyright notice in the Description page of Project Settings.

#include "UI/WidgetController/BottomController.h"
#include "UI/Panel/RightPanel.h"
#include "UI/WidgetController/RightPanelController.h"

void UBottomController::InitializeWithRightPanel(URightPanel* InRightPanel)
{
	RightPanel = InRightPanel;
	CurrentPanelType = ERightPanelType::Scene; // 기본값: Scene

	UE_LOG(LogTemp, Log, TEXT("BottomController: Initialized with RightPanel"));
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
	UE_LOG(LogTemp, Warning, TEXT("BottomController: TogglePanel"));
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
