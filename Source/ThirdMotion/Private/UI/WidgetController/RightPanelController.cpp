// Fill out your copyright notice in the Description page of Project Settings.

#include "UI/WidgetController/RightPanelController.h"
#include "UI/Panel/RightPanel.h"

void URightPanelController::InitializeWithRightPanel(URightPanel* InRightPanel)
{
	if (!InRightPanel)
	{
		UE_LOG(LogTemp, Warning, TEXT("RightPanelController: RightPanel is null"));
		return;
	}

	RightPanel = InRightPanel;
	CurrentPanel = ERightPanelType::Scene; // 기본값: Scene
	bIsPanelVisible = false; // 초기에는 숨김

	// RightPanel 초기 상태: 숨김
	RightPanel->SetRightPanelVisibility(false);

	UE_LOG(LogTemp, Log, TEXT("RightPanelController: Initialized with RightPanel (hidden)"));
}

void URightPanelController::SwitchToPanel(ERightPanelType PanelType)
{
	if (!RightPanel)
	{
		UE_LOG(LogTemp, Warning, TEXT("RightPanelController: RightPanel is null"));
		return;
	}

	// 패널 타입 업데이트
	CurrentPanel = PanelType;
	bIsPanelVisible = true;

	// RightPanel 보이기
	RightPanel->SetRightPanelVisibility(true);

	// RightPanel에 WidgetSwitcher 인덱스 변경 요청
	int32 PanelIndex = static_cast<int32>(PanelType);
	RightPanel->SetWidgetSwitcherIndex(PanelIndex);

	// 패널별 초기화
	switch (PanelType)
	{
	case ERightPanelType::Scene:
		InitializeScenePanel();
		break;
	case ERightPanelType::Properties:
		InitializePropertiesPanel();
		break;
	case ERightPanelType::XYZ:
		InitializeXYZPanel();
		break;
	case ERightPanelType::UserList:
		InitializeUserListPanel();
		break;
	}

	UE_LOG(LogTemp, Log, TEXT("RightPanelController: Switched to panel %d"), PanelIndex);
}

void URightPanelController::TogglePanel(ERightPanelType PanelType)
{
	if (!RightPanel)
	{
		UE_LOG(LogTemp, Warning, TEXT("RightPanelController: RightPanel is null"));
		return;
	}

	// 같은 패널을 다시 클릭하면 숨김
	if (bIsPanelVisible && CurrentPanel == PanelType)
	{
		HidePanel();
	}
	else
	{
		// 다른 패널이거나 숨겨진 상태면 전환
		SwitchToPanel(PanelType);
	}
}

void URightPanelController::HidePanel()
{
	if (!RightPanel)
	{
		UE_LOG(LogTemp, Warning, TEXT("RightPanelController: RightPanel is null"));
		return;
	}

	bIsPanelVisible = false;
	RightPanel->SetRightPanelVisibility(false);

	UE_LOG(LogTemp, Log, TEXT("RightPanelController: Panel hidden"));
}

void URightPanelController::InitializeScenePanel()
{
	// Scene 패널 초기화 로직
	UE_LOG(LogTemp, Log, TEXT("RightPanelController: Scene panel initialized"));
}

void URightPanelController::InitializePropertiesPanel()
{
	// Properties 패널 초기화 로직
	UE_LOG(LogTemp, Log, TEXT("RightPanelController: Properties panel initialized"));
}

void URightPanelController::InitializeXYZPanel()
{
	// XYZ 패널 초기화 로직
	UE_LOG(LogTemp, Log, TEXT("RightPanelController: XYZ panel initialized"));
}

void URightPanelController::InitializeUserListPanel()
{
	// UserList 패널 초기화 로직
	UE_LOG(LogTemp, Log, TEXT("RightPanelController: UserList panel initialized"));
}
