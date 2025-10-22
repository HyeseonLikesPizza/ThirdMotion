// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UI/Panel/MaterialGeneratePanel.h"
#include "UI/Widget/ListingMaterials.h"
#include "UObject/Object.h"
#include "MaterialThumbnailData.generated.h"

/**
 * 
 */
UCLASS()
class THIRDMOTION_API UMaterialThumbnailData : public UObject
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Material")
	FString MaterialName;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Material")
	UTexture2D* Thumbnail;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Material")
	EMaterialType MaterialType;
};
