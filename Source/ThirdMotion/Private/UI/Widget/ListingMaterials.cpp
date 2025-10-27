// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/Widget/ListingMaterials.h"

#include "Components/Image.h"
#include "Components/TextBlock.h"
#include "Data/MaterialPreviewData.h"

#include "Components/Image.h"
#include "Engine/TextureRenderTarget2D.h"
#include "Slate/SlateBrushAsset.h"

void UListingMaterials::NativeConstruct()
{
	Super::NativeConstruct();
}

void UListingMaterials::NativeDestruct()
{
	Super::NativeDestruct();
}

// void UListingMaterials::NativeOnListItemObjectSet(UObject* ListItemObject)
// {
// 	IUserObjectListEntry::NativeOnListItemObjectSet(ListItemObject);
//
// 	// Super::NativeOnListItemObjectSet(ListItemObject);
// 	
// 	
// 	MaterialData = Cast<UMaterialPreviewData>(ListItemObject);
// 	if (!MaterialData) return;
// 	
// 	// 텍스트와 썸네일 표시 (BindWidget으로 선언되어 있다고 가정)
// 	if (MaterialNameText)
// 		MaterialNameText->SetText(FText::FromString(MaterialData->MaterialName));
// 	
// 	if (PreviewImage && MaterialData->RenderTarget && RenderTargetMaterial)
// 	{
// 		UMaterialInstanceDynamic* DynMaterial = PreviewImage->GetDynamicMaterial();
// 		if (DynMaterial == nullptr)
// 		{
// 			DynMaterial = UMaterialInstanceDynamic::Create(RenderTargetMaterial, this);
// 			PreviewImage->SetBrushFromMaterial(DynMaterial);
// 		}
// 		DynMaterial->SetTextureParameterValue("RenderTarget", MaterialData->RenderTarget);
// 	}
// }

void UListingMaterials::NativeOnListItemObjectSet(UObject* ListItemObject)
{
	IUserObjectListEntry::NativeOnListItemObjectSet(ListItemObject);

	// Super::NativeOnListItemObjectSet(ListItemObject);
	
	
	MaterialData = Cast<UMaterialPreviewData>(ListItemObject);
	if (!MaterialData) return;
	
	// 텍스트와 썸네일 표시 (BindWidget으로 선언되어 있다고 가정)
	if (MaterialNameText)
		MaterialNameText->SetText(FText::FromString(MaterialData->MaterialName));
	
	if (PreviewImage && MaterialData->RenderTarget)
	{
		UMaterialInterface* Mat = PreviewImage->GetDynamicMaterial();
		UE_LOG(LogTemp, Log, TEXT("Listing Material %s"), *MaterialData->MaterialName);


		
		// Image의 브러시를 RenderTarget으로 세팅
		FSlateBrush Brush;
		Brush.SetResourceObject(MaterialData->RenderTarget);
		Brush.ImageSize = FVector2D(128, 128);
		PreviewImage->SetBrush(Brush);
	}
}

FReply UListingMaterials::NativeOnMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{

	if (MaterialData)
	{
		UE_LOG(LogTemp, Warning, TEXT("Canvas Tile Clicked: %s"), *MaterialData->MaterialName);
	}

	
	
	//버튼이 눌리면 우측 디테일창에 해당 메터리얼인스턴스의 정보값이 표시된다

	// 클릭 이벤트가 처리되었다고 엔진에 알림
	return FReply::Handled();
}

// void UListingMaterials::NativeOnListItemObjectSet(UObject* ListItemObject)
// {
// 	IUserObjectListEntry::NativeOnListItemObjectSet(ListItemObject);
//
// 	// Super::NativeOnListItemObjectSet(ListItemObject);
// 	
// 	
