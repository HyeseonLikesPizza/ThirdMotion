
#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "UI/Widget/BaseWidget.h"
#include "RightPanel.generated.h"

class UTreeView;
class USceneItemData;
class AActor;
class USceneItemWidget;

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

	// SceneItemWidget 클래스 (Blueprint에서 설정 또는 C++에서 지정)
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Scene List")
	TSubclassOf<UUserWidget> SceneItemWidgetClass;

	// Row 생성 델리게이트 (Outliner처럼 동적 생성)
	UFUNCTION()
	UUserWidget* OnGenerateRow(UObject* Item);

protected:
	virtual void NativeConstruct() override;
	virtual void NativeDestruct() override;
	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;

	// TreeView 델리게이트
	void OnGetItemChildren(UObject* Item, TArray<UObject*>& Children);

	// Blueprint 이벤트 핸들러
	UFUNCTION()
	void OnItemExpansionChangedEvent(UObject* Item, bool bIsExpanded);

	UFUNCTION()
	void OnItemSelectionChangedEvent(UObject* Item, bool bIsSelected);

private:
	// 데이터 관리
	UPROPERTY()
	TArray<USceneItemData*> RootItems;

	UPROPERTY()
	TMap<AActor*, USceneItemData*> ActorToItemMap;

	float RefreshTimer = 0.0f;
	bool bIsUpdating = false;

	// 기능 함수들
	void RefreshSceneActors();
    
	// 선택 동기화
	void SyncWithViewport();
    
#if WITH_EDITOR
	void OnEditorSelectionChanged(UObject* NewSelection);
	void SyncSelectionFromEditor();
	bool bIsSelectingFromList = false;
#endif
};
