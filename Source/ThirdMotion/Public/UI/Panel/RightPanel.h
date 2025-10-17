
#pragma once

#include "CoreMinimal.h"
#include "UI/Widget/BaseWidget.h"
#include "Components/TreeView.h"
#include "Engine/Light.h"
#include "Camera/CameraActor.h"
#include "EngineUtils.h"
#include "RightPanel.generated.h"

class USceneItemData;
class UEditableTextBox;

UCLASS()
class THIRDMOTION_API URightPanel : public UBaseWidget
{
	GENERATED_BODY()

public:
	virtual void NativeConstruct() override;
	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;

protected:
	UPROPERTY(meta = (BindWidget))
	UTreeView* SceneTreeView;

	UPROPERTY(meta = (BindWidget))
	UEditableTextBox* SearchBox;

	// 씬 액터 목록 업데이트
	void RefreshSceneActors();

	// 액터 계층 구조 구축
	void BuildActorHierarchy();

	// TreeView 델리게이트
	UFUNCTION()
	void OnGetItemChildren(UObject* Item, TArray<UObject*>& Children);

	UFUNCTION()
	void OnItemExpansionChanged(UObject* Item, bool bIsExpanded);

	UFUNCTION()
	void OnItemSelectionChanged(UObject* Item);

private:
	UPROPERTY()
	TArray<USceneItemData*> RootItems;

	UPROPERTY()
	TMap<AActor*, USceneItemData*> ActorToItemMap;

	float RefreshTimer = 0.0f;

};
