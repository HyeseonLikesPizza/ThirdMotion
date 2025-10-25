
#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "UI/Widget/BaseWidget.h"
#include "RightPanel.generated.h"

class UTreeView;
class USceneController;
class USceneList;
class USceneItemWidget;
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
 * - Scene 관련: SceneController + SceneList
 */
UCLASS()
class THIRDMOTION_API URightPanel : public UBaseWidget
{
	GENERATED_BODY()

public:
	// ==================== UI 위젯 ====================

	UPROPERTY(meta = (BindWidget))
	UTreeView* SceneList;

	UPROPERTY(meta = (BindWidget))
	class UTextBlock* PanelTitle;

	// WidgetSwitcher for panel switching (Library, Scene, Properties)
	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	class UWidgetSwitcher* WidgetSwitcher_Right;

	// SceneItemWidget 클래스
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Scene List")
	TSubclassOf<UUserWidget> SceneItemWidgetClass;

	// ==================== Public Interface ====================

	// Row 생성 (TreeView 델리게이트)
	UFUNCTION()
	UUserWidget* OnGenerateRow(UObject* Item);

	// SceneController 접근자 (외부에서 접근 가능)
	UFUNCTION(BlueprintCallable, Category = "Scene Panel")
	USceneController* GetSceneController() const { return SceneController; }

	// SceneList 접근자 (외부에서 접근 가능)
	UFUNCTION(BlueprintCallable, Category = "Scene Panel")
	USceneList* GetSceneListData() const { return SceneListData; }

	// RightPanelController 접근자
	UFUNCTION(BlueprintCallable, Category = "Right Panel")
	URightPanelController* GetRightPanelController() const { return RightPanelController; }

	// WidgetSwitcher 인덱스 변경 (View 기능만)
	UFUNCTION(BlueprintCallable, Category = "Right Panel")
	void SetWidgetSwitcherIndex(int32 Index);

	// RightPanel 가시성 설정 (View 기능만)
	UFUNCTION(BlueprintCallable, Category = "Right Panel")
	void SetRightPanelVisibility(bool bVisible);

protected:
	virtual void NativeConstruct() override;
	virtual void NativeDestruct() override;
	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;

	// TreeView 델리게이트
	void OnGetItemChildren(UObject* Item, TArray<UObject*>& Children);

	// TreeView 이벤트 핸들러
	UFUNCTION()
	void OnItemSelectionChangedEvent(UObject* Item, bool bIsSelected);

private:
	// RightPanelController (패널 전환 및 비즈니스 로직)
	UPROPERTY()
	URightPanelController* RightPanelController;

	// SceneController (Scene 패널 전용)
	UPROPERTY()
	USceneController* SceneController;

	// SceneList (Scene 패널 데이터)
	UPROPERTY()
	class USceneList* SceneListData;

	float RefreshTimer = 0.0f;

	// 초기화
	void InitializeRightPanelController();
	void InitializeSceneController();
	void InitializeSceneList();

	// UI 업데이트
	void RefreshUI();
};
