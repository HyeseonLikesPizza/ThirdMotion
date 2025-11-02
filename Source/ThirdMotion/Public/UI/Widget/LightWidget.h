// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UI/Widget/BaseWidget.h"
#include "LightWidget.generated.h"

class ULightController;
class USlider;
class UTextBlock;
class UComboBoxString;

/**
 * LightWidget - Light Actor의 Intensity와 Color 편집 UI (MVC View)
 */
UCLASS()
class THIRDMOTION_API ULightWidget : public UBaseWidget
{
	GENERATED_BODY()

public:
	virtual void NativeConstruct() override;
	virtual void NativeDestruct() override;

	// LightController와 Light Actor로 초기화
	UFUNCTION(BlueprintCallable, Category = "Light Widget")
	void InitializeWithLightActor(AActor* InLightActor);

	// LightController 접근자
	UFUNCTION(BlueprintCallable, Category = "Light Widget")
	ULightController* GetLightController() const { return LightController; }

	// Widget 가시성 설정
	UFUNCTION(BlueprintCallable, Category = "Light Widget")
	void SetLightWidgetVisibility(bool bVisible);

	// UI Components - Intensity
	UPROPERTY(BlueprintReadWrite, meta = (BindWidgetOptional))
	USlider* IntensitySlider;

	UPROPERTY(BlueprintReadWrite, meta = (BindWidgetOptional))
	UTextBlock* IntensityText;

	// UI Components - Color (Blueprint에서 ColorPicker로 구현)
	UPROPERTY(BlueprintReadWrite, meta = (BindWidgetOptional))
	class UButton* ColorPickerButton;

	// UI Components - Color Combo (Red, Yellow, Green)
	UPROPERTY(BlueprintReadWrite, meta = (BindWidgetOptional))
	UComboBoxString* ColorCombo;

protected:
	// LightController (MVC Controller)
	UPROPERTY()
	ULightController* LightController;

	// Slider 변경 이벤트
	UFUNCTION()
	void OnIntensitySliderChanged(float Value);

	// ColorCombo 선택 변경 이벤트
	UFUNCTION()
	void OnColorComboSelectionChanged(FString SelectedItem, ESelectInfo::Type SelectionType);

	// Observer 패턴 - Controller 델리게이트 콜백
	UFUNCTION()
	void OnLightIntensityChanged(float NewIntensity);

	UFUNCTION()
	void OnLightColorChanged(FLinearColor NewColor);

	// UI 업데이트
	void UpdateIntensityUI(float Intensity);
};
