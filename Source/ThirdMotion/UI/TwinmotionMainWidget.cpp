// TwinmotionMainWidget.cpp
// Copyright Epic Games, Inc. All Rights Reserved.

#include "UI/TwinmotionMainWidget.h"
#include "TwinmotionUIManager.h"
#include "Components/Widget.h"

void UTwinmotionMainWidget::NativeConstruct()
{
	Super::NativeConstruct();

	if (UIManager)
	{
		// Subscribe to panel visibility changes
		UIManager->OnPanelVisibilityChanged.AddDynamic(this, &UTwinmotionMainWidget::OnPanelVisibilityChanged);
	}
}

void UTwinmotionMainWidget::ToggleLeftPanel()
{
	if (UIManager)
	{
		bool bCurrentVisible = UIManager->IsPanelVisible(ETwinmotionPanelType::Library);
		UIManager->SetPanelVisibility(ETwinmotionPanelType::Library, !bCurrentVisible);
	}
}

void UTwinmotionMainWidget::ToggleRightPanel()
{
	if (UIManager)
	{
		bool bCurrentVisible = UIManager->IsPanelVisible(ETwinmotionPanelType::Scene);
		UIManager->SetPanelVisibility(ETwinmotionPanelType::Scene, !bCurrentVisible);
	}
}

void UTwinmotionMainWidget::ToggleBottomPanel()
{
	if (UIManager)
	{
		bool bCurrentVisible = UIManager->IsPanelVisible(ETwinmotionPanelType::Thumbnail);
		UIManager->SetPanelVisibility(ETwinmotionPanelType::Thumbnail, !bCurrentVisible);
	}
}

void UTwinmotionMainWidget::OnPanelVisibilityChanged(ETwinmotionPanelType PanelType, bool bVisible)
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
