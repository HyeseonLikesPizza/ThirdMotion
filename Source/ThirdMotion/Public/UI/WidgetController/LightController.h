// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UI/WidgetController/BaseWidgetController.h"
#include "LightController.generated.h"

// Light 속성 변경 이벤트 델리게이트 (Observer 패턴)
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnLightIntensityChanged, float, NewIntensity);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnLightColorChanged, FLinearColor, NewColor);

/**
 * LightController - Light Actor의 Intensity와 Color 관리 (MVC Controller + Observer 패턴)
 */
UCLASS()
class THIRDMOTION_API ULightController : public UBaseWidgetController
{
	GENERATED_BODY()

public:
	// 선택된 Light Actor 초기화
	UFUNCTION(BlueprintCallable, Category = "Light Controller")
	void InitializeWithLightActor(AActor* InLightActor);

	// Intensity 변경 (UI에서 호출)
	UFUNCTION(BlueprintCallable, Category = "Light Controller")
	void SetLightIntensity(float NewIntensity);

	// Color 변경 (UI에서 호출)
	UFUNCTION(BlueprintCallable, Category = "Light Controller")
	void SetLightColor(FLinearColor NewColor);

	// 현재 값 가져오기
	UFUNCTION(BlueprintCallable, Category = "Light Controller")
	float GetCurrentIntensity() const { return CurrentIntensity; }

	UFUNCTION(BlueprintCallable, Category = "Light Controller")
	FLinearColor GetCurrentColor() const { return CurrentColor; }

	// Observer 패턴 - 델리게이트
	UPROPERTY(BlueprintAssignable, Category = "Light Controller Events")
	FOnLightIntensityChanged OnLightIntensityChanged;

	UPROPERTY(BlueprintAssignable, Category = "Light Controller Events")
	FOnLightColorChanged OnLightColorChanged;

	// Light Actor 가져오기
	UFUNCTION(BlueprintCallable, Category = "Light Controller")
	AActor* GetLightActor() const { return LightActor.Get(); }

protected:
	// 선택된 Light Actor
	UPROPERTY()
	TWeakObjectPtr<AActor> LightActor;

	// 현재 Intensity
	float CurrentIntensity;

	// 현재 Color
	FLinearColor CurrentColor;

	// SceneManager를 통해 서버에 변경 요청
	void RequestServerUpdate(const struct FPropertyDelta& Delta);
};
