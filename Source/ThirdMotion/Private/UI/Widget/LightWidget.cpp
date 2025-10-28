// Fill out your copyright notice in the Description page of Project Settings.

#include "UI/Widget/LightWidget.h"
#include "UI/WidgetController/LightController.h"
#include "Components/Slider.h"
#include "Components/TextBlock.h"
#include "Components/Button.h"

void ULightWidget::NativeConstruct()
{
	Super::NativeConstruct();

	// Slider 이벤트 바인딩
	if (IntensitySlider)
	{
		IntensitySlider->OnValueChanged.AddDynamic(this, &ULightWidget::OnIntensitySliderChanged);
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
