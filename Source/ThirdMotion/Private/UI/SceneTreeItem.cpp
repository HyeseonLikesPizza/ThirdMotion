// TwinmotionSceneTreeItem.cpp
// Copyright Epic Games, Inc. All Rights Reserved.

#include "UI/SceneTreeItem.h"
#include "Components/Button.h"
#include "Components/Image.h"
#include "Components/EditableText.h"
#include "Components/Spacer.h"

void USceneTreeItem::NativeConstruct()
{
	Super::NativeConstruct();

	// Bind button events
	if (ExpandButton)
	{
		ExpandButton->OnClicked.AddDynamic(this, &USceneTreeItem::OnExpandButtonClicked);
	}

	if (VisibilityButton)
	{
		VisibilityButton->OnClicked.AddDynamic(this, &USceneTreeItem::OnVisibilityButtonClicked);
	}

	if (SelectButton)
	{
		SelectButton->OnClicked.AddDynamic(this, &USceneTreeItem::OnSelectButtonClicked);
	}

	if (NameText)
	{
		NameText->OnTextCommitted.AddDynamic(this, &USceneTreeItem::OnNameTextCommitted);
	}

	bIsSelected = false;
}

void USceneTreeItem::SetSceneTreeItemData(const FSceneTreeItemData& InData)
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

void USceneTreeItem::SetExpanded(bool bExpanded)
{
	ItemData.bIsExpanded = bExpanded;
	UpdateExpandButton();
	OnItemExpandedChanged(bExpanded);
	OnTreeItemExpanded.Broadcast(ItemData, bExpanded);

	UE_LOG(LogTemp, Log, TEXT("Tree item %s: %s"),
		*ItemData.Name, bExpanded ? TEXT("expanded") : TEXT("collapsed"));
}

void USceneTreeItem::SetItemVisible(bool bVisible)
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

void USceneTreeItem::SetSelected(bool bSelected)
{
	bIsSelected = bSelected;
	OnItemSelectedChanged(bSelected);

	UE_LOG(LogTemp, Log, TEXT("Tree item %s: %s"),
		*ItemData.Name, bSelected ? TEXT("selected") : TEXT("deselected"));
}

void USceneTreeItem::OnExpandButtonClicked()
{
	SetExpanded(!ItemData.bIsExpanded);
}

void USceneTreeItem::OnVisibilityButtonClicked()
{
	SetItemVisible(!ItemData.bIsVisible);
}

void USceneTreeItem::OnSelectButtonClicked()
{
	SetSelected(true);
	OnTreeItemSelected.Broadcast(ItemData);
\
}

void USceneTreeItem::OnNameTextCommitted(const FText& Text, ETextCommit::Type CommitMethod)
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

void USceneTreeItem::UpdateIndentation()
{
	// This would typically add a Spacer widget to create indentation
	// The exact implementation depends on your widget structure
	if (ItemContainer)
	{
		// Spacer size = IndentLevel * 16px
		// This should be done in Blueprint or by dynamically adding spacers
	}
}

void USceneTreeItem::UpdateExpandButton()
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

void USceneTreeItem::UpdateVisibilityButton()
{
	if (VisibilityButton)
	{
		// Update button icon based on visibility state
		// This should be implemented in Blueprint using OnItemVisibilityChanged
	}
}
