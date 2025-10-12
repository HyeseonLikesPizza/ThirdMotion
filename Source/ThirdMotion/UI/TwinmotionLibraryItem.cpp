// TwinmotionLibraryItem.cpp
// Copyright Epic Games, Inc. All Rights Reserved.

#include "UI/TwinmotionLibraryItem.h"
#include "Components/Image.h"
#include "Components/TextBlock.h"
#include "Components/Button.h"

void UTwinmotionLibraryItem::NativeConstruct()
{
	Super::NativeConstruct();

	if (ItemButton)
	{
		ItemButton->OnClicked.AddDynamic(this, &UTwinmotionLibraryItem::OnButtonClicked);
	}

	bIsSelected = false;
}

void UTwinmotionLibraryItem::SetLibraryItemData(const FLibraryItemData& InData)
{
	ItemData = InData;

	// Update visual components
	if (ThumbnailImage && ItemData.Thumbnail)
	{
		ThumbnailImage->SetBrushFromTexture(ItemData.Thumbnail);
	}

	if (ItemNameText)
	{
		ItemNameText->SetText(FText::FromString(ItemData.Name));
	}

	UE_LOG(LogTemp, Log, TEXT("Library item set: %s (Type: %d)"),
		*ItemData.Name, static_cast<int32>(ItemData.Type));
}

void UTwinmotionLibraryItem::OnButtonClicked()
{
	OnLibraryItemClicked.Broadcast(ItemData);
	UE_LOG(LogTemp, Log, TEXT("Library item clicked: %s"), *ItemData.Name);
}

void UTwinmotionLibraryItem::SetSelected(bool bSelected)
{
	bIsSelected = bSelected;
	OnItemSelected(bSelected);

	UE_LOG(LogTemp, Log, TEXT("Library item %s: %s"),
		bSelected ? TEXT("selected") : TEXT("deselected"), *ItemData.Name);
}
