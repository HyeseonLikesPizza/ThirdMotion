// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/Widget/ListingMaterials.h"

#include "Components/Image.h"
#include "Components/TextBlock.h"
#include "Data/MaterialThumbnailData.h"

void UListingMaterials::NativeConstruct()
{
	Super::NativeConstruct();
}

void UListingMaterials::NativeDestruct()
{
	Super::NativeDestruct();
}

void UListingMaterials::NativeOnListItemObjectSet(UObject* ListItemObject)
{
	IUserObjectListEntry::NativeOnListItemObjectSet(ListItemObject);

	// Super::NativeOnListItemObjectSet(ListItemObject);
	
	
	UMaterialThumbnailData* ItemData = Cast<UMaterialThumbnailData>(ListItemObject);
	if (!ItemData) return;
	
	// 텍스트와 썸네일 표시 (BindWidget으로 선언되어 있다고 가정)
	if (MaterialNameText)
		MaterialNameText->SetText(FText::FromString(ItemData->MaterialName));
	
	if (ThumbnailImage && ItemData->Thumbnail)
		ThumbnailImage->SetBrushFromTexture(ItemData->Thumbnail);
}


