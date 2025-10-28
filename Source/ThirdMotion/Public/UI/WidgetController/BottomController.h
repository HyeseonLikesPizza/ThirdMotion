// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UI/WidgetController/BaseWidgetController.h"
#include "BottomController.generated.h"

class URightPanel;
class UBottomView;

// RightPanel 타입 열거형
UENUM(BlueprintType)
enum class ERightPanelType : uint8
{
	Scene UMETA(DisplayName = "Scene"),
	Properties UMETA(DisplayName = "Properties"),
	XYZ UMETA(DisplayName = "XYZ"),
	UserList UMETA(DisplayName = "UserList")
};

// BottomView 타입 열거형
UENUM(BlueprintType)
enum class EBottomViewType : uint8
{
	None UMETA(DisplayName = "None"),
	Material UMETA(DisplayName = "Material"),
	Memo UMETA(DisplayName = "Memo")
};

// 패널 변경 이벤트 델리게이트
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnRightPanelChanged, ERightPanelType, NewPanelType);

// BottomView 변경 이벤트 델리게이트
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnBottomViewChanged, EBottomViewType, NewViewType);

/**
 * BottomController - BottomBar와 RightPanel 상호작용 관리
 */
UCLASS()
class THIRDMOTION_API UBottomController : public UBaseWidgetController
{
	GENERATED_BODY()

public:
	// RightPanel 참조로 초기화
	void InitializeWithRightPanel(URightPanel* InRightPanel);

	// BottomView 참조로 초기화
	void InitializeWithBottomView(UBottomView* InBottomView);

	// 패널 토글 메서드 (같은 패널 클릭 시 숨김)
	UFUNCTION(BlueprintCallable, Category = "Bottom Controller")
	void ToggleScenePanel();

	UFUNCTION(BlueprintCallable, Category = "Bottom Controller")
	void TogglePropertiesPanel();

	UFUNCTION(BlueprintCallable, Category = "Bottom Controller")
	void ToggleXYZPanel();

	UFUNCTION(BlueprintCallable, Category = "Bottom Controller")
	void ToggleUserListPanel();

	// BottomView 토글 메서드 (같은 뷰 클릭 시 숨김)
	UFUNCTION(BlueprintCallable, Category = "Bottom Controller")
	void ToggleMaterialView();

	UFUNCTION(BlueprintCallable, Category = "Bottom Controller")
	void ToggleMemoView();

	// 패널 변경 이벤트
	UPROPERTY(BlueprintAssignable, Category = "Bottom Controller Events")
	FOnRightPanelChanged OnRightPanelChanged;

	// BottomView 변경 이벤트
	UPROPERTY(BlueprintAssignable, Category = "Bottom Controller Events")
	FOnBottomViewChanged OnBottomViewChanged;

	// 현재 패널 타입 가져오기
	UFUNCTION(BlueprintCallable, Category = "Bottom Controller")
	ERightPanelType GetCurrentPanelType() const { return CurrentPanelType; }

	// 현재 BottomView 타입 가져오기
	UFUNCTION(BlueprintCallable, Category = "Bottom Controller")
	EBottomViewType GetCurrentViewType() const { return CurrentViewType; }

protected:
	// RightPanel 참조
	UPROPERTY()
	URightPanel* RightPanel;

	// BottomView 참조
	UPROPERTY()
	UBottomView* BottomView;

	// 현재 활성 패널
	ERightPanelType CurrentPanelType;

	// 현재 활성 BottomView
	EBottomViewType CurrentViewType;

	// 패널 토글 로직
	void TogglePanel(ERightPanelType PanelType);

	// BottomView 토글 로직
	void ToggleView(EBottomViewType ViewType);
};
