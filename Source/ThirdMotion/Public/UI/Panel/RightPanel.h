
#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "UI/Widget/BaseWidget.h"
#include "RightPanel.generated.h"

class UTreeView;
class USceneController;
class USceneList;
class USceneItemWidget;
class AActor;

// Forward declaration
class USceneItemData;

/**
 * RightPanel - View 역할 (MVC Pattern)
 *
 * 책임:
 * - UI 표시만 담당 (TreeView, 위젯 업데이트)
 * - 사용자 입력을 SceneController에게 위임
 * - Model(SceneList)의 데이터 변경 알림을 받아 UI 업데이트
 *
 * 역할 분리:
 * - View: UI 표시 및 사용자 입력 수신
 * - Controller: SceneController가 모든 비즈니스 로직 처리
 * - Model: SceneList가 데이터 관리
 *
 * 데이터 흐름:
 * 1. User Input -> RightPanel(View) -> SceneController(Controller)
 * 2. SceneController -> SceneList(Model) 데이터 업데이트
 * 3. SceneList -> Observer 알림 -> RightPanel(View) UI 업데이트
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
	// SceneController (모든 액션 처리)
	UPROPERTY()
	USceneController* SceneController;

	// SceneList (데이터 관리)
	UPROPERTY()
	class USceneList* SceneListData;

	float RefreshTimer = 0.0f;

	// 초기화
	void InitializeController();
	void InitializeSceneList();

	// 델리게이트 콜백
	UFUNCTION()
	void OnSceneDataChanged();

	UFUNCTION()
	void OnActorAddedToScene(USceneItemData* AddedItem);

	UFUNCTION()
	void OnActorRemovedFromScene(AActor* RemovedActor);

	UFUNCTION()
	void OnSelectionChanged(const TArray<AActor*>& SelectedActors);

	// UI 업데이트
	void RefreshUI();

#if WITH_EDITOR
	void OnEditorSelectionChanged(UObject* NewSelection);
	void SyncSelectionFromEditor();
	bool bIsSelectingFromList = false;
#endif
};
