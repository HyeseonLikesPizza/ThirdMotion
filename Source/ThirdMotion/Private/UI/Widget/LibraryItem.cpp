// TwinmotionLibraryItem.cpp
// Copyright Epic Games, Inc. All Rights Reserved.

#include "UI/Widget/LibraryItem.h"
#include "Components/Image.h"
#include "Components/TextBlock.h"
#include "Components/Button.h"
#include "Data/LibraryItemObject.h"

void ULibraryItem::NativeConstruct()
{
	Super::NativeConstruct();

	if (ItemButton)
	{
		ItemButton->OnClicked.AddDynamic(this, &ULibraryItem::OnButtonClicked);
	}

	bIsSelected = false;
}

void ULibraryItem::NativeOnListItemObjectSet(UObject* ListItem)
{
	IUserObjectListEntry::NativeOnListItemObjectSet(ListItem);

	if (const auto* Data = Cast<ULibraryItemObject>(ListItem))
	{
		ItemNameText->SetText(Data->DisplayName);

		// 아이콘 로드
		if (!Data->Icon.IsNull())
		{
			if (UTexture2D* Tex = Data->Icon.LoadSynchronous())
				ThumbnailImage->SetBrushFromTexture(Tex, true);
		}
	}
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
