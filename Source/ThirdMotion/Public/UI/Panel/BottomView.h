// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UI/Widget/BaseWidget.h"
#include "BottomView.generated.h"

// BottomView 타입 열거형
UENUM(BlueprintType)
enum class EBottomViewType : uint8
{
	None UMETA(DisplayName = "None"),
	Material UMETA(DisplayName = "Material"),
	Memo UMETA(DisplayName = "Memo")
};

/**
 * BottomView - 하단 뷰 (Material, Memo 등)
 */
UCLASS()
class THIRDMOTION_API UBottomView : public UBaseWidget
{
	GENERATED_BODY()

public:
	// WidgetSwitcher for view switching
	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	class UWidgetSwitcher* BottomViewSwitcher;

	// BottomViewSwitcher 인덱스 변경
	UFUNCTION(BlueprintCallable, Category = "Bottom View")
	void SetBottomViewSwitcherIndex(int32 Index);

	// BottomView 가시성 설정
	UFUNCTION(BlueprintCallable, Category = "Bottom View")
	void SetBottomViewVisibility(bool bVisible);
};
