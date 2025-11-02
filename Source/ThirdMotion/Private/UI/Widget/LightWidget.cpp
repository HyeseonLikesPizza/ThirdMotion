// Fill out your copyright notice in the Description page of Project Settings.

#include "UI/Widget/LightWidget.h"
#include "UI/WidgetController/LightController.h"
#include "Components/Slider.h"
#include "Components/TextBlock.h"
#include "Components/Button.h"
#include "Components/ComboBoxString.h"

void ULightWidget::NativeConstruct()
{
	Super::NativeConstruct();

	// Slider 이벤트 바인딩
	if (IntensitySlider)
	{
		IntensitySlider->OnValueChanged.AddDynamic(this, &ULightWidget::OnIntensitySliderChanged);
	}

	// ColorCombo 초기화
	if (ColorCombo)
	{
		ColorCombo->ClearOptions();
		ColorCombo->AddOption(TEXT("Red"));
		ColorCombo->AddOption(TEXT("Yellow"));
		ColorCombo->AddOption(TEXT("Green"));
		ColorCombo->SetSelectedOption(TEXT("Red")); // 기본값: Red

		ColorCombo->OnSelectionChanged.AddDynamic(this, &ULightWidget::OnColorComboSelectionChanged);

		UE_LOG(LogTemp, Log, TEXT("LightWidget: ColorCombo initialized with Red, Yellow, Green"));
	}

	UE_LOG(LogTemp, Log, TEXT("LightWidget: NativeConstruct completed"));
}

void ULightWidget::NativeDestruct()
{
	// 델리게이트 언바인딩 (메모리 누수 방지)
	if (LightController)
	{
		LightController->OnLightIntensityChanged.RemoveDynamic(this, &ULightWidget::OnLightIntensityChanged);
		LightController->OnLightColorChanged.RemoveDynamic(this, &ULightWidget::OnLightColorChanged);
	}

	if (IntensitySlider)
	{
		IntensitySlider->OnValueChanged.RemoveDynamic(this, &ULightWidget::OnIntensitySliderChanged);
	}

	if (ColorCombo)
	{
		ColorCombo->OnSelectionChanged.RemoveDynamic(this, &ULightWidget::OnColorComboSelectionChanged);
	}

	Super::NativeDestruct();
}

void ULightWidget::InitializeWithLightActor(AActor* InLightActor)
{
	if (!InLightActor)
	{
		UE_LOG(LogTemp, Warning, TEXT("LightWidget: Light Actor is null"));
		return;
	}

	// LightController 생성 및 초기화
	if (!LightController)
	{
		LightController = NewObject<ULightController>(this);
	}

	LightController->InitializeWithLightActor(InLightActor);

	// Observer 패턴 - Controller 델리게이트 바인딩
	LightController->OnLightIntensityChanged.AddDynamic(this, &ULightWidget::OnLightIntensityChanged);
	LightController->OnLightColorChanged.AddDynamic(this, &ULightWidget::OnLightColorChanged);

	// 초기 UI 업데이트
	UpdateIntensityUI(LightController->GetCurrentIntensity());

	UE_LOG(LogTemp, Log, TEXT("LightWidget: Initialized with Light Actor"));
}

void ULightWidget::SetLightWidgetVisibility(bool bVisible)
{
	if (bVisible)
	{
		SetVisibility(ESlateVisibility::Visible);
	}
	else
	{
		SetVisibility(ESlateVisibility::Collapsed);
	}
}

void ULightWidget::OnIntensitySliderChanged(float Value)
{
	if (!LightController)
		return;

	// Controller를 통해 변경 요청
	LightController->SetLightIntensity(Value);
}

void ULightWidget::OnColorComboSelectionChanged(FString SelectedItem, ESelectInfo::Type SelectionType)
{
	if (!LightController)
		return;

	// 선택된 색상에 따라 FLinearColor 생성
	FLinearColor NewColor = FLinearColor::White;

	if (SelectedItem == TEXT("Red"))
	{
		NewColor = FLinearColor(1.0f, 0.0f, 0.0f, 1.0f); // Red
	}
	else if (SelectedItem == TEXT("Yellow"))
	{
		NewColor = FLinearColor(1.0f, 1.0f, 0.0f, 1.0f); // Yellow
	}
	else if (SelectedItem == TEXT("Green"))
	{
		NewColor = FLinearColor(0.0f, 1.0f, 0.0f, 1.0f); // Green
	}

	// Controller를 통해 변경 요청
	LightController->SetLightColor(NewColor);

	UE_LOG(LogTemp, Log, TEXT("LightWidget: ColorCombo selection changed to %s"), *SelectedItem);
}

void ULightWidget::OnLightIntensityChanged(float NewIntensity)
{
	// Observer 패턴 - Controller에서 변경 알림
	UpdateIntensityUI(NewIntensity);
	UE_LOG(LogTemp, Log, TEXT("LightWidget: Intensity updated to %f"), NewIntensity);
}

void ULightWidget::OnLightColorChanged(FLinearColor NewColor)
{
	// Observer 패턴 - Controller에서 변경 알림
	// TODO: ColorPicker UI 업데이트 (Blueprint에서 구현)
	UE_LOG(LogTemp, Log, TEXT("LightWidget: Color updated to %s"), *NewColor.ToString());
}

void ULightWidget::UpdateIntensityUI(float Intensity)
{
	// Slider 업데이트
	if (IntensitySlider)
	{
		IntensitySlider->SetValue(Intensity);
	}

	// Text 업데이트
	if (IntensityText)
	{
		FText IntensityStr = FText::AsNumber(Intensity);
		IntensityText->SetText(IntensityStr);
	}
}
