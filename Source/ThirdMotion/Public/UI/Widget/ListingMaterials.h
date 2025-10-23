// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BaseWidget.h"
#include "Blueprint/IUserObjectListEntry.h"
#include "ListingMaterials.generated.h"

/**
 * 
 */



UCLASS()
class THIRDMOTION_API UListingMaterials : public UBaseWidget, public IUserObjectListEntry
{
	GENERATED_BODY()

public:
	virtual void NativeConstruct() override;
	virtual void NativeDestruct() override;
	virtual void NativeOnListItemObjectSet(UObject* ListItemObject) override;




	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Material")
	FString MaterialName;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Material")
	UTexture2D* MaterialThumbnail;


	UPROPERTY(meta = (BindWidget))
	class UTextBlock* MaterialNameText;

	UPROPERTY(meta = (BindWidget))
	class UImage* ThumbnailImage;

	//선택된 메터리얼 타입을 담을 변수
	
	
	// 사용중 여부

	//메터리얼 썸네일

	//메테리얼 이름 기본 (기본이름 설정되어 있음)
	

};
