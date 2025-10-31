// Fill out your copyright notice in the Description page of Project Settings.

#include "UI/Panel/BottomView.h"
#include "Components/WidgetSwitcher.h"
#include "UI/Panel/RightPanel.h"
#include "UI/WidgetController/RightPanelController.h"
#include "UI/WidgetController/BottomController.h"

void UBottomView::SetBottomViewSwitcherIndex(int32 Index)
{
	if (!BottomViewSwitcher)
	{
		UE_LOG(LogTemp, Warning, TEXT("BottomView: BottomViewSwitcher is null"));
		return;
	}

	// WidgetSwitcher 인덱스 변경
	BottomViewSwitcher->SetActiveWidgetIndex(Index);

	UE_LOG(LogTemp, Log, TEXT("BottomView: BottomViewSwitcher set to index %d"), Index);
}

void UBottomView::SetBottomViewVisibility(bool bVisible)
{
	if (bVisible)
	{
		SetVisibility(ESlateVisibility::Visible);
		UE_LOG(LogTemp, Log, TEXT("BottomView: Panel shown"));
	}
	else
	{
		SetVisibility(ESlateVisibility::Collapsed);
		UE_LOG(LogTemp, Log, TEXT("BottomView: Panel hidden"));
	}
}

void UBottomView::InitializeWithRightPanel(URightPanel* InRightPanel)
{
	RightPanel = InRightPanel;

	if (RightPanel)
	{
		UE_LOG(LogTemp, Log, TEXT("BottomView: RightPanel initialized"));
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("BottomView: RightPanel is null"));
	}
}

void UBottomView::ToggleRightPanel(int32 PanelIndex)
{
	if (!RightPanel)
	{
		UE_LOG(LogTemp, Warning, TEXT("BottomView: RightPanel is not initialized, cannot toggle"));
		return;
	}

	// RightPanelController를 통해 패널 토글
	if (URightPanelController* Controller = RightPanel->GetRightPanelController())
	{
		// PanelIndex를 ERightPanelType으로 변환하여 토글
		ERightPanelType PanelType = static_cast<ERightPanelType>(PanelIndex);
		Controller->TogglePanel(PanelType);
		UE_LOG(LogTemp, Log, TEXT("BottomView: Toggled RightPanel index %d"), PanelIndex);
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("BottomView: RightPanelController is null"));
	}
}
