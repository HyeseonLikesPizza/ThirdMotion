// TwinmotionMainWidget.cpp
// Copyright Epic Games, Inc. All Rights Reserved.

#include "UI/MainWidget.h"
#include "UI/UIManager.h"
#include "Components/Widget.h"

void UMainWidget::NativeConstruct()
{
	Super::NativeConstruct();

	if (UIManager)
	{
		// Subscribe to panel visibility changes
		UIManager->OnPanelVisibilityChanged.AddDynamic(this, &UMainWidget::OnPanelVisibilityChanged);
	}
}

void UMainWidget::ToggleLeftPanel()
{
	if (UIManager)
	{
		bool bCurrentVisible = UIManager->IsPanelVisible(ETwinmotionPanelType::Library);
		UIManager->SetPanelVisibility(ETwinmotionPanelType::Library, !bCurrentVisible);
	}
}

void UMainWidget::ToggleRightPanel()
{
	if (UIManager)
	{
		bool bCurrentVisible = UIManager->IsPanelVisible(ETwinmotionPanelType::Scene);
		UIManager->SetPanelVisibility(ETwinmotionPanelType::Scene, !bCurrentVisible);
	}
}

void UMainWidget::ToggleBottomPanel()
{
	if (UIManager)
	{
		bool bCurrentVisible = UIManager->IsPanelVisible(ETwinmotionPanelType::Thumbnail);
		UIManager->SetPanelVisibility(ETwinmotionPanelType::Thumbnail, !bCurrentVisible);
	}
}

void UMainWidget::OnPanelVisibilityChanged(ETwinmotionPanelType PanelType, bool bVisible)
{
	ESlateVisibility NewVisibility = bVisible ? ESlateVisibility::Visible : ESlateVisibility::Collapsed;

	switch (PanelType)
	{
		case ETwinmotionPanelType::Library:
			if (LeftPanel)
			{
				LeftPanel->SetVisibility(NewVisibility);
			}
			break;

		case ETwinmotionPanelType::Scene:
		case ETwinmotionPanelType::Properties:
			if (RightPanel)
			{
				RightPanel->SetVisibility(NewVisibility);
			}
			break;

		case ETwinmotionPanelType::Thumbnail:
			if (BottomPanel)
			{
				BottomPanel->SetVisibility(NewVisibility);
			}
		break;
	}
}
