
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
 * RightPanel - View 역할
 * UI 표시만 담당
 * SceneController와 SceneList를 사용하여 데이터 표시
 */
UCLASS()
class THIRDMOTION_API URightPanel : public UBaseWidget
{
	GENERATED_BODY()

public:
	// UI 위젯
	UPROPERTY(meta = (BindWidget))
	UTreeView* SceneList;

	UPROPERTY(meta = (BindWidget))
	class UTextBlock* PanelTitle;

	// SceneItemWidget 클래스
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Scene List")
	TSubclassOf<UUserWidget> SceneItemWidgetClass;

	// Row 생성
	UFUNCTION()
	UUserWidget* OnGenerateRow(UObject* Item);

	// SceneController 접근자
	UFUNCTION(BlueprintCallable, Category = "Scene Panel")
	USceneController* GetSceneController() const { return SceneController; }

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
	void OnSelectionChanged(const TArray<AActor*>& SelectedActors);

	// UI 업데이트
	void RefreshUI();

#if WITH_EDITOR
	void OnEditorSelectionChanged(UObject* NewSelection);
	void SyncSelectionFromEditor();
	bool bIsSelectingFromList = false;
#endif
};
