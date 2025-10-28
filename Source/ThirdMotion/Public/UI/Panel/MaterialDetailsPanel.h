// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "MaterialDetailsPanel.generated.h"

/**
 * 
 */
UCLASS()
class THIRDMOTION_API UMaterialDetailsPanel : public UUserWidget
{
	GENERATED_BODY()

public:
	UPROPERTY(meta=(BindWidget))
	class UTextBlock* MaterialNameText;

	UPROPERTY(meta=(BindWidget))
	class UImage* MaterialPreviewImage;


public:
	UFUNCTION()
	void TurnOnPanel();
	
	UFUNCTION()
	void SetNameAndPreviewImg(class UMaterialDetailsData* materialDetailsData);
	
};
