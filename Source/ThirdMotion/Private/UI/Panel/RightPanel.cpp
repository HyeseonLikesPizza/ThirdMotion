// RightPanel.cpp
// Copyright Epic Games, Inc. All Rights Reserved.

#include "UI/Panel/RightPanel.h"
#include "UI/Widget/SceneTreeItem.h"
#include "UI/WidgetController/ScenePanelController.h"
#include "Components/ScrollBox.h"
#include "Components/TextBlock.h"
#include "Blueprint/WidgetTree.h"

void URightPanel::NativeConstruct()
{
	Super::NativeConstruct();

	if (PanelTitle)
	{
		PanelTitle->SetText(FText::FromString(TEXT("Scene")));
	}

	UE_LOG(LogTemp, Log, TEXT("RightPanel constructed"));
}

void URightPanel::NativeDestruct()
{
	// Unbind from controller events
	if (ScenePanelController)
	{
		ScenePanelController->OnSceneTreeRefreshed.RemoveDynamic(this, &URightPanel::OnSceneTreeRefreshed);
		ScenePanelController->OnActorSelectedInScene.RemoveDynamic(this, &URightPanel::OnActorSelectedInScene);
	}

	Super::NativeDestruct();
}

void URightPanel::InitializePanel(UScenePanelController* InController)
{
	if (!InController)
	{
		UE_LOG(LogTemp, Error, TEXT("RightPanel: Controller is null!"));
		return;
	}

	ScenePanelController = InController;

	// Bind to controller events (Observer Pattern)
	ScenePanelController->OnSceneTreeRefreshed.AddDynamic(this, &URightPanel::OnSceneTreeRefreshed);
	ScenePanelController->OnActorSelectedInScene.AddDynamic(this, &URightPanel::OnActorSelectedInScene);

	UE_LOG(LogTemp, Log, TEXT("RightPanel initialized with controller"));

	// Initial refresh
	RefreshSceneList();
}

void URightPanel::ShowScenePanel()
{
	SetVisibility(ESlateVisibility::Visible);
	RefreshSceneList();

	UE_LOG(LogTemp, Log, TEXT("Scene panel shown"));
}

void URightPanel::HidePanel()
{
	SetVisibility(ESlateVisibility::Collapsed);

	UE_LOG(LogTemp, Log, TEXT("Right panel hidden"));
}

void URightPanel::RefreshSceneList()
{
	if (!ScenePanelController)
	{
		UE_LOG(LogTemp, Warning, TEXT("RightPanel: No controller assigned"));
		return;
	}

	ScenePanelController->RefreshSceneTree();
}

void URightPanel::OnSceneTreeRefreshed(const TArray<FSceneTreeItemData>& SceneItems)
{
	ClearSceneList();
	PopulateSceneList(SceneItems);

	UE_LOG(LogTemp, Log, TEXT("RightPanel: Scene tree refreshed with %d items"), SceneItems.Num());
}

void URightPanel::OnActorSelectedInScene(AActor* SelectedActor)
{
	if (SelectedActor)
	{
		UE_LOG(LogTemp, Log, TEXT("RightPanel: Actor selected - %s"), *SelectedActor->GetName());
	}
}

void URightPanel::ClearSceneList()
{
	if (!SceneListContainer) return;

	// Remove all child widgets
	SceneListContainer->ClearChildren();
	SceneTreeItems.Empty();

	UE_LOG(LogTemp, Log, TEXT("Scene list cleared"));
}

void URightPanel::PopulateSceneList(const TArray<FSceneTreeItemData>& SceneItems)
{
	if (!SceneListContainer) return;

	if (!SceneTreeItemClass)
	{
		UE_LOG(LogTemp, Error, TEXT("RightPanel: SceneTreeItemClass not set!"));
		return;
	}

	// Create widget for each scene item
	for (const FSceneTreeItemData& ItemData : SceneItems)
	{
		USceneTreeItem* TreeItemWidget = CreateTreeItemWidget(ItemData);
		if (TreeItemWidget)
		{
			SceneListContainer->AddChild(TreeItemWidget);
			SceneTreeItems.Add(TreeItemWidget);

			// Bind tree item to controller (Observer Pattern)
			if (ScenePanelController)
			{
				ScenePanelController->BindToTreeItem(TreeItemWidget);
			}
		}
	}

	UE_LOG(LogTemp, Log, TEXT("Scene list populated with %d items"), SceneTreeItems.Num());
}

USceneTreeItem* URightPanel::CreateTreeItemWidget(const FSceneTreeItemData& ItemData)
{
	if (!SceneTreeItemClass) return nullptr;

	USceneTreeItem* TreeItemWidget = CreateWidget<USceneTreeItem>(this, SceneTreeItemClass);
	if (TreeItemWidget)
	{
		TreeItemWidget->SetSceneTreeItemData(ItemData);
		return TreeItemWidget;
	}

	return nullptr;
}
