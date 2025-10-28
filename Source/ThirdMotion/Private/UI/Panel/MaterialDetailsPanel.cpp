// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/Panel/MaterialDetailsPanel.h"

#include "Components/Image.h"
#include "Components/TextBlock.h"
#include "Data/MaterialDetailsData.h"
#include "Data/MaterialPreviewData.h"
#include "Engine/TextureRenderTarget2D.h"


// Super::NativeOnListItemObjectSet(ListItemObject);


void UMaterialDetailsPanel::TurnOnPanel()
{
	SetVisibility(ESlateVisibility::Visible);
}

void UMaterialDetailsPanel::SetNameAndPreviewImg(class UMaterialDetailsData* materialDetailsData)
{
	// MaterialData = Cast<UMaterialPreviewData>(ListItemObject);
	// if (!MaterialData) return;
	
	// 텍스트와 썸네일 표시 (BindWidget으로 선언되어 있다고 가정)
	// if (MaterialNameText)
	
	
	MaterialNameText->SetText(FText::FromString(materialDetailsData->MaterialName));
	
	if (MaterialPreviewImage && materialDetailsData->RenderTarget)
	{
		// Image의 브러시를 RenderTarget으로 세팅
		FSlateBrush Brush;
		Brush.SetResourceObject(materialDetailsData->RenderTarget);
		Brush.ImageSize = FVector2D(128, 128);
		MaterialPreviewImage->SetBrush(Brush);
	}
}
