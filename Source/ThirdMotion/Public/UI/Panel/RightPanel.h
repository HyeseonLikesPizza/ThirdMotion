
#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "UI/Widget/BaseWidget.h"
#include "RightPanel.generated.h"

class UXYZWidget;
class UMeshSettingsWidget;
class ULightWidget;
class UTreeView;
class USceneController;
class USceneList;
class USceneItemWidget;
class USceneListWidget;
class URightPanelController;
class AActor;

// Forward declaration
class USceneItemData;

/**
 * RightPanel - View 역할 (MVC Pattern)
 *
 * 책임:
 * - UI 표시만 담당 (WidgetSwitcher, TreeView 등)
 * - 사용자 입력을 RightPanelController에게 위임
 * - 비즈니스 로직은 RightPanelController가 처리
 *
 * 역할 분리:
 * - View (RightPanel): UI 표시만 담당
 * - Controller (RightPanelController): 패널 전환 및 비즈니스 로직
 */
UCLASS()
class THIRDMOTION_API URightPanel : public UBaseWidget
{
	GENERATED_BODY()

public:
	// ==================== UI 위젯 ====================

	// WidgetSwitcher for panel switching (Library, Scene, Properties)
	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	class UWidgetSwitcher* WidgetSwitcher_Right;

	// Properties 패널
	UPROPERTY(meta = (BindWidget))
	UMeshSettingsWidget* MeshSettingsWidget;

	// Light 편집 패널
	UPROPERTY(meta = (BindWidgetOptional))
	ULightWidget* LightWidget;

	// Scene 패널
	UPROPERTY(meta = (BindWidget))
	USceneListWidget* SceneListWidget;

	// XYZ 패널
	UPROPERTY(meta = (BindWidget))
	UXYZWidget* XYZWidget;

	// ==================== Public Interface ====================

	// RightPanelController 접근자
	UFUNCTION(BlueprintCallable, Category = "Right Panel")
	URightPanelController* GetRightPanelController() const { return RightPanelController; }

	// SceneController 접근자 (SceneListWidget을 통해 접근)
	UFUNCTION(BlueprintCallable, Category = "Right Panel")
	USceneController* GetSceneController() const;

	// WidgetSwitcher 인덱스 변경 (View 기능만)
	UFUNCTION(BlueprintCallable, Category = "Right Panel")
	void SetWidgetSwitcherIndex(int32 Index);

	// RightPanel 가시성 설정 (View 기능만)
	UFUNCTION(BlueprintCallable, Category = "Right Panel")
	void SetRightPanelVisibility(bool bVisible);

protected:
	virtual void NativeConstruct() override;
	virtual void NativeDestruct() override;

private:
	// RightPanelController (패널 전환 및 비즈니스 로직)
	UPROPERTY()
	URightPanelController* RightPanelController;

	// 초기화
	void InitializeRightPanelController();

	// Actor 선택 처리 (Observer 패턴)
	UFUNCTION()
	void OnActorSelected(AActor* SelectedActor);

	// Light Actor 체크 및 LightWidget 표시
	void HandleLightActorSelection(AActor* LightActor);

};
