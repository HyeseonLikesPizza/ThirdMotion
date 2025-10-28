
#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "SceneListWidget.generated.h"

class UTreeView;
class UTextBlock;
class USceneController;
class USceneList;
class USceneItemWidget;
class URightPanelController;
class URightPanel;
class USceneItemData;
class AActor;

/**
 *
 *  * - Scene 관련: SceneController + SceneList
 */
UCLASS()
class THIRDMOTION_API USceneListWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	
	UPROPERTY(meta = (BindWidget))
	UTreeView* SceneList;

	UPROPERTY(meta = (BindWidget))
	class UTextBlock* PanelTitle;

	// SceneItemWidget 클래스
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Scene List")
	TSubclassOf<UUserWidget> SceneItemWidgetClass;

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
	virtual FReply NativeOnKeyDown(const FGeometry& InGeometry, const FKeyEvent& InKeyEvent) override;

	// TreeView 델리게이트
	void OnGetItemChildren(UObject* Item, TArray<UObject*>& Children);

	// TreeView 이벤트 핸들러
	UFUNCTION()
	void OnItemSelectionChangedEvent(UObject* Item, bool bIsSelected);

	// SceneList 데이터 변경 이벤트 핸들러
	UFUNCTION()
	void OnSceneListDataChanged();

	// World에서 Actor 선택 시 이벤트 핸들러
	UFUNCTION()
	void OnActorSelectedInWorld(const TArray<AActor*>& SelectedActors);

private:
	UPROPERTY()
	URightPanelController* RightPanelController;
	UPROPERTY()
	URightPanel* RightPanel;
	
	// SceneController (Scene 패널 전용)
	UPROPERTY()
	USceneController* SceneController;

	// SceneList (Scene 패널 데이터)
	UPROPERTY()
	class USceneList* SceneListData;

	float RefreshTimer = 0.0f;

	void InitializeSceneController();
	void InitializeSceneList();
	
	// UI 업데이트
	void RefreshUI();

};
