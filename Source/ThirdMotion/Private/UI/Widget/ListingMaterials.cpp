// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/Widget/ListingMaterials.h"

#include "Components/Image.h"
#include "Components/TextBlock.h"
#include "Data/MaterialPreviewData.h"

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
	
	
	MaterialData = Cast<UMaterialPreviewData>(ListItemObject);
	if (!MaterialData) return;
	
	// 텍스트와 썸네일 표시 (BindWidget으로 선언되어 있다고 가정)
	if (MaterialNameText)
		MaterialNameText->SetText(FText::FromString(MaterialData->MaterialName));
	
	if (ThumbnailImage && MaterialData->PreviewImage)
		ThumbnailImage->SetBrushFromTexture(MaterialData->PreviewImage);
}

FReply UListingMaterials::NativeOnMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{

	if (MaterialData)
	{
		UE_LOG(LogTemp, Warning, TEXT("Canvas Tile Clicked: %s"), *MaterialData->MaterialName);
	}

	// 클릭 이벤트가 처리되었다고 엔진에 알림
	return FReply::Handled();
}


