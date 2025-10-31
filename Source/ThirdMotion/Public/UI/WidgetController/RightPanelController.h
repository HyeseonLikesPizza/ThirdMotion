// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UI/WidgetController/BaseWidgetController.h"
#include "UI/WidgetController/BottomController.h"
#include "RightPanelController.generated.h"

class UMeshWidgetController;
class URightPanel;

/**
 * RightPanelController - RightPanel의 비즈니스 로직 처리
 *
 * 책임:
 * - 패널 전환 로직 관리
 * - WidgetSwitcher 제어
 * - 패널별 데이터 관리 및 처리
 */
UCLASS()
class THIRDMOTION_API URightPanelController : public UBaseWidgetController
{
	GENERATED_BODY()

public:
	// RightPanel 참조로 초기화
	void InitializeWithRightPanel(URightPanel* InRightPanel);

	// 패널 전환 (BottomController에서 호출)
	UFUNCTION(BlueprintCallable, Category = "Right Panel Controller")
	void SwitchToPanel(ERightPanelType PanelType);

	// 패널 전환 (커스텀 인덱스 지정)
	void SwitchToPanel(ERightPanelType PanelType, int32 CustomIndex);

	// 패널 토글 (같은 패널 클릭 시 숨김)
	UFUNCTION(BlueprintCallable, Category = "Right Panel Controller")
	void TogglePanel(ERightPanelType PanelType);

	// RightPanel 숨기기
	UFUNCTION(BlueprintCallable, Category = "Right Panel Controller")
	void HidePanel();

	// 현재 활성 패널 가져오기
	UFUNCTION(BlueprintCallable, Category = "Right Panel Controller")
	ERightPanelType GetCurrentPanel() const { return CurrentPanel; }

	// RightPanel이 보이는지 확인
	UFUNCTION(BlueprintCallable, Category = "Right Panel Controller")
	bool IsPanelVisible() const { return bIsPanelVisible; }

	// MaterialGeneratePanel과 MeshWidgetController 바인딩 헬퍼 함수
	UFUNCTION(BlueprintCallable)
	void BindMaterialPanel(UMaterialGeneratePanel* InView);

protected:
	// RightPanel 참조 (View)
	UPROPERTY()
	URightPanel* RightPanel;

	// 현재 활성 패널
	ERightPanelType CurrentPanel;

	// 패널 표시 상태
	bool bIsPanelVisible;

	// 패널별 초기화 로직
	void InitializeScenePanel();
	void InitializePropertiesPanel();
	void InitializeUserListPanel();

	// PresetType에 따라 PropertiesSwitcher 인덱스 설정 (헬퍼 함수)
	void UpdatePropertiesSwitcherByActor(AActor* SelectedActor);

private:
	UPROPERTY()
	UMeshWidgetController* MeshWidgetController;
};
