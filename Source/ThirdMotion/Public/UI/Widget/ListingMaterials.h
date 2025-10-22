// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BaseWidget.h"
#include "ListingMaterials.generated.h"

/**
 * 
 */

UENUM(BlueprintType)
enum class EMaterialType : uint8
{
	Tire UMETA(DisplayName = "Tire"),
	Foliage UMETA(DisplayName = "Foliage"),
	Video UMETA(DisplayName = "Video"),
	Subsurface UMETA(DisplayName = "Subsurface"),
	CarPaint UMETA(DisplayName = "CarPaint"),
	Water UMETA(DisplayName = "Water"),
	Emissive UMETA(DisplayName = "Emissive"),
	Fabric UMETA(DisplayName = "Fabric"),
	Glass UMETA(DisplayName = "Glass"),
	Standard UMETA(DisplayName = "Standard")
};

UCLASS()
class THIRDMOTION_API UListingMaterials : public UBaseWidget
{
	GENERATED_BODY()

public:
	virtual void NativeConstruct() override;
	virtual void NativeDestruct() override;

	// UPROPERTY()
	// UEnum* MatEnumPtr = StaticEnum<EMaterialType>();


	//선택된 메터리얼 타입을 담을 변수
	
	
	// 사용중 여부

	//메터리얼 썸네일

	//메테리얼 이름 기본 (기본이름 설정되어 있음)
	

};
