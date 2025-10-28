// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UI/Panel/MaterialGeneratePanel.h"
#include "UObject/Object.h"
#include "MaterialDetailsData.generated.h"

/**
 * 
 */
UCLASS()
class THIRDMOTION_API UMaterialDetailsData : public UObject
{
	GENERATED_BODY()
public:

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Material")
	FString MaterialName;

	// UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Material")
	// UTexture2D* PreviewImage;
	//
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Material")
	class UTextureRenderTarget2D* RenderTarget;


	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Material")
	EMaterialType MaterialType;
};
