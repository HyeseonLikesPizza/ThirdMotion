// TwinmotionSceneTreeItem.cpp
// Copyright Epic Games, Inc. All Rights Reserved.

#include "UI/TwinmotionSceneTreeItem.h"
#include "Components/Button.h"
#include "Components/Image.h"
#include "Components/EditableText.h"
#include "Components/Spacer.h"
#include "TwinmotionUIManager.h"

void UTwinmotionSceneTreeItem::NativeConstruct()
{
	Super::NativeConstruct();

	// Bind button events
	if (ExpandButton)
	{
		ExpandButton->OnClicked.AddDynamic(this, &UTwinmotionSceneTreeItem::OnExpandButtonClicked);
	}

	if (VisibilityButton)
	{
		VisibilityButton->OnClicked.AddDynamic(this, &UTwinmotionSceneTreeItem::OnVisibilityButtonClicked);
	}

	if (SelectButton)
	{
		SelectButton->OnClicked.AddDynamic(this, &UTwinmotionSceneTreeItem::OnSelectButtonClicked);
	}

	if (NameText)
	{
		NameText->OnTextCommitted.AddDynamic(this, &UTwinmotionSceneTreeItem::OnNameTextCommitted);
	}

	bIsSelected = false;
}

void UTwinmotionSceneTreeItem::SetSceneTreeItemData(const FSceneTreeItemData& InData)
{
	ItemData = InData;

	// Update name
	if (NameText)
	{
		NameText->SetText(FText::FromString(ItemData.Name));
	}

	// Update icon
	if (IconImage && ItemData.Icon)
	{
		IconImage->SetBrushFromTexture(ItemData.Icon);
	}

	// Update indentation
	UpdateIndentation();

	// Update expand button
	UpdateExpandButton();

	// Update visibility button
	UpdateVisibilityButton();

	UE_LOG(LogTemp, Log, TEXT("Scene tree item set: %s (Indent: %d, HasChildren: %d)"),
		*ItemData.Name, ItemData.IndentLevel, ItemData.bHasChildren);
}

void UTwinmotionSceneTreeItem::SetExpanded(bool bExpanded)
{
	ItemData.bIsExpanded = bExpanded;
	UpdateExpandButton();
	OnItemExpandedChanged(bExpanded);
	OnTreeItemExpanded.Broadcast(ItemData, bExpanded);

	UE_LOG(LogTemp, Log, TEXT("Tree item %s: %s"),
		*ItemData.Name, bExpanded ? TEXT("expanded") : TEXT("collapsed"));
}

void UTwinmotionSceneTreeItem::SetItemVisible(bool bVisible)
{
	ItemData.bIsVisible = bVisible;
	UpdateVisibilityButton();
	OnItemVisibilityChanged(bVisible);
	OnTreeItemVisibilityChanged.Broadcast(ItemData, bVisible);

	// Update actual actor visibility if available
	if (ItemData.Actor)
	{
		ItemData.Actor->SetActorHiddenInGame(!bVisible);
		ItemData.Actor->SetActorEnableCollision(bVisible);
	}

	UE_LOG(LogTemp, Log, TEXT("Tree item %s visibility: %s"),
		*ItemData.Name, bVisible ? TEXT("visible") : TEXT("hidden"));
}

void UTwinmotionSceneTreeItem::SetSelected(bool bSelected)
{
	bIsSelected = bSelected;
	OnItemSelectedChanged(bSelected);

	UE_LOG(LogTemp, Log, TEXT("Tree item %s: %s"),
		*ItemData.Name, bSelected ? TEXT("selected") : TEXT("deselected"));
}

void UTwinmotionSceneTreeItem::OnExpandButtonClicked()
{
	SetExpanded(!ItemData.bIsExpanded);
}

void UTwinmotionSceneTreeItem::OnVisibilityButtonClicked()
{
	SetItemVisible(!ItemData.bIsVisible);
}

void UTwinmotionSceneTreeItem::OnSelectButtonClicked()
{
	SetSelected(true);
	OnTreeItemSelected.Broadcast(ItemData);

	// Notify UI Manager
	if (UIManager && ItemData.Actor)
	{
		UIManager->SelectSceneObject(ItemData.Actor);
	}

	UE_LOG(LogTemp, Log, TEXT("Tree item selected: %s"), *ItemData.Name);
}

void UTwinmotionSceneTreeItem::OnNameTextCommitted(const FText& Text, ETextCommit::Type CommitMethod)
{
	if (CommitMethod == ETextCommit::OnEnter || CommitMethod == ETextCommit::OnUserMovedFocus)
	{
		FString NewName = Text.ToString();
		if (NewName != ItemData.Name && !NewName.IsEmpty())
		{
			FString OldName = ItemData.Name;
			ItemData.Name = NewName;

			// Rename actual actor if available
			if (ItemData.Actor)
			{
				ItemData.Actor->Rename(*NewName);
			}

			OnTreeItemRenamed.Broadcast(ItemData, NewName);
			UE_LOG(LogTemp, Log, TEXT("Tree item renamed: %s -> %s"), *OldName, *NewName);
		}
	}
}

void UTwinmotionSceneTreeItem::UpdateIndentation()
{
	// This would typically add a Spacer widget to create indentation
	// The exact implementation depends on your widget structure
	if (ItemContainer)
	{
		// Spacer size = IndentLevel * 16px
		// This should be done in Blueprint or by dynamically adding spacers
	}
}

void UTwinmotionSceneTreeItem::UpdateExpandButton()
{
	if (ExpandButton)
	{
		// Show/hide expand button based on whether item has children
		ESlateVisibility NewVisibility = ItemData.bHasChildren ?
			ESlateVisibility::Visible : ESlateVisibility::Hidden;

		ExpandButton->SetVisibility(NewVisibility);

		// Update button icon based on expanded state
		// This should be implemented in Blueprint using OnItemExpandedChanged
	}
}

void UTwinmotionSceneTreeItem::UpdateVisibilityButton()
{
	if (VisibilityButton)
	{
		// Update button icon based on visibility state
		// This should be implemented in Blueprint using OnItemVisibilityChanged
	}
}
