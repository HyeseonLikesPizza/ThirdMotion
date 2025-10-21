// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UI/Widget/BaseWidget.h"
#include "MaterialGeneratePanel.generated.h"

/**
 * 
 */

UENUM(BlueprintType)
enum class EMaterialType : uint8
{
	Tire UMETA(DisplayName = "Tire"),
	Foliage UMETA(DisplayName = "Foliage"),
	Video UMETA(DisplayName = "Subsurface"),
	Subsurface UMETA(DisplayName = "Subsurface"),
	CarPaint UMETA(DisplayName = "CarPaint"),
	Water UMETA(DisplayName = "Water"),
	Emissive UMETA(DisplayName = "Emissive"),
	Fabric UMETA(DisplayName = "Fabric"),
	Glass UMETA(DisplayName = "Glass"),
	Standard UMETA(DisplayName = "Standard")
};

UCLASS()
class THIRDMOTION_API UMaterialGeneratePanel : public UBaseWidget
{
	GENERATED_BODY()

public:
	virtual bool Initialize() override;

protected:

	//canvas
	UPROPERTY(meta=(BindWidget))
	class UCanvasPanel* MaterialTypeCanvas;
	
	// //vertical Box
	// UPROPERTY(meta=(BindWidget))
	// class UVerticalBox* MaterialTypeSelectionBtnListBox;

	
	// UPROPERTY(meta = (BindWidget))
	// class UButton* MaterialTypeSelectionBtn;

	

private:
	//메터리얼 정렬 필터 버튼 함수

	//안쓰이는 메터리얼 삭제 함수


	UFUNCTION()
	void CreateMaterialTypeEnumBtns();
	
	//Material 기본 재질 설정 버튼 리스트...버튼 함수
	UFUNCTION()
	void OnMaterialTypeSelectionBtnClicked();

	//Material 생성 함수
};
