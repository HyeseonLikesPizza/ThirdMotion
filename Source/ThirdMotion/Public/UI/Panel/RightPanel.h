
#pragma once

#include "CoreMinimal.h"
#include "UI/Widget/BaseWidget.h"
#include "Components/ListView.h"
#include "Components/ScrollBox.h"
#include "Components/TextBlock.h"
#include "UI/WidgetController/ScenePanelController.h"
#include "RightPanel.generated.h"

class USceneTreeItem;
class UScenePanelController;

UCLASS()
class THIRDMOTION_API URightPanel : public UBaseWidget
{
	GENERATED_BODY()

public:
	virtual void NativeConstruct() override;
	virtual void NativeDestruct() override;

	// Initialize with controller
	UFUNCTION(BlueprintCallable, Category = "Right Panel")
	void InitializePanel(UScenePanelController* InController);

	// Show/Hide panel
	UFUNCTION(BlueprintCallable, Category = "Right Panel")
	void ShowScenePanel();

	UFUNCTION(BlueprintCallable, Category = "Right Panel")
	void HidePanel();

	// Refresh scene list
	UFUNCTION(BlueprintCallable, Category = "Right Panel")
	void RefreshSceneList();

	// Widget Components
	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	UTextBlock* PanelTitle;

	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	UScrollBox* SceneListContainer;

	// Scene Tree Item Class
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Right Panel")
	TSubclassOf<USceneTreeItem> SceneTreeItemClass;

	// Widget Controller
	UPROPERTY()
	UScenePanelController* ScenePanelController;

	// Created tree items
	UPROPERTY()
	TArray<USceneTreeItem*> SceneTreeItems;

	// Controller event callbacks
	UFUNCTION()
	void OnSceneTreeRefreshed(const TArray<FSceneTreeItemData>& SceneItems);

	UFUNCTION()
	void OnActorSelectedInScene(AActor* SelectedActor);

	// Helper functions
	void ClearSceneList();
	void PopulateSceneList(const TArray<FSceneTreeItemData>& SceneItems);
	USceneTreeItem* CreateTreeItemWidget(const FSceneTreeItemData& ItemData);
};
