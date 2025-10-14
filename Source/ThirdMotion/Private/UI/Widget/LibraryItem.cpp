// TwinmotionLibraryItem.cpp
// Copyright Epic Games, Inc. All Rights Reserved.

#include "UI/Widget/LibraryItem.h"
#include "Components/Image.h"
#include "Components/TextBlock.h"
#include "Components/Button.h"

void ULibraryItem::NativeConstruct()
{
	Super::NativeConstruct();

	if (ItemButton)
	{
		ItemButton->OnClicked.AddDynamic(this, &ULibraryItem::OnButtonClicked);
	}

	bIsSelected = false;
}

void ULibraryItem::SetLibraryItemData(const FLibraryItemData& InData)
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

void ULibraryItem::OnButtonClicked()
{
	OnLibraryItemClicked.Broadcast(ItemData);
	UE_LOG(LogTemp, Log, TEXT("Library item clicked: %s"), *ItemData.Name);
}

void ULibraryItem::SetSelected(bool bSelected)
{
	bIsSelected = bSelected;
	OnItemSelected(bSelected);

	UE_LOG(LogTemp, Log, TEXT("Library item %s: %s"),
		bSelected ? TEXT("selected") : TEXT("deselected"), *ItemData.Name);
}
