// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UI/Widget/BaseWidget.h"
#include "Components/TextBlock.h"
#include "Components/ScrollBox.h"
#include "ActorDetailsPanel.generated.h"

class UPropertyRow;
class UActorDetailsController;
class UActorDetailsData;

/**
 * ActorDetailsPanel - View 역할 (MVC Pattern)
 *
 * 책임:
 * - UI 표시만 담당 (PropertyRow 위젯 업데이트)
 * - 사용자 입력을 ActorDetailsController에게 위임
 * - Model(ActorDetailsData)의 데이터 변경 알림을 받아 UI 업데이트
 *
 * 역할 분리:
 * - View: UI 표시 및 사용자 입력 수신
 * - Controller: ActorDetailsController가 모든 비즈니스 로직 처리
 * - Model: ActorDetailsData가 데이터 관리
 *
 * 데이터 흐름:
 * 1. User Input -> ActorDetailsPanel(View) -> ActorDetailsController(Controller)
 * 2. ActorDetailsController -> ActorDetailsData(Model) 데이터 업데이트
 * 3. ActorDetailsData -> Observer 알림 -> ActorDetailsPanel(View) UI 업데이트
 */
UCLASS()
class THIRDMOTION_API UActorDetailsPanel : public UBaseWidget
{
	GENERATED_BODY()

public:
	virtual void NativeConstruct() override;
	virtual void NativeDestruct() override;

	// Widget Components
	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	UTextBlock* ActorNameText;

	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	UTextBlock* ActorClassText;

	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	UScrollBox* PropertiesScrollBox;

	// PropertyRow 위젯 클래스
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Actor Details")
	TSubclassOf<UPropertyRow> PropertyRowClass;

	// Controller 접근자
	UFUNCTION(BlueprintCallable, Category = "Actor Details")
	UActorDetailsController* GetDetailsController() const { return DetailsController; }

	// Model 접근자
	UFUNCTION(BlueprintCallable, Category = "Actor Details")
	UActorDetailsData* GetDetailsData() const { return DetailsData; }

protected:
	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;

private:
	// Controller (모든 액션 처리)
	UPROPERTY()
	UActorDetailsController* DetailsController;

	// Model (데이터 관리)
	UPROPERTY()
	UActorDetailsData* DetailsData;

	// 생성된 PropertyRow 위젯들
	UPROPERTY()
	TArray<UPropertyRow*> PropertyRows;

	// 초기화
	void InitializeController();
	void InitializeDetailsData();

	// Observer 콜백: Model의 데이터 변경 알림
	UFUNCTION()
	void OnDetailsDataChanged(UActorDetailsData* ChangedData);

	// UI 업데이트 (View의 역할)
	void RefreshUI();
	void ClearUI();
	void UpdateProperties();
};
