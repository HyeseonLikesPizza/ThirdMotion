// ScenePanelController.h - Scene Panel Widget Controller
// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "UI/WidgetController/BaseWidgetController.h"
#include "UI/Widget/SceneTreeItem.h"
#include "ScenePanelController.generated.h"

class USceneTreeItem;
class AActor;

/**
 * Controller for Scene Panel - manages scene tree hierarchy display
 * Observer Pattern: Listens to SceneTreeItem events and updates scene accordingly
 */
UCLASS()
class THIRDMOTION_API UScenePanelController : public UBaseWidgetController
{
	GENERATED_BODY()

public:
	virtual void Init() override;

	// Scene Tree Management
	UFUNCTION(BlueprintCallable, Category = "Scene Panel")
	void RefreshSceneTree();

	UFUNCTION(BlueprintCallable, Category = "Scene Panel")
	TArray<FSceneTreeItemData> GetSceneActors();

	UFUNCTION(BlueprintCallable, Category = "Scene Panel")
	void SelectActor(AActor* Actor);

	UFUNCTION(BlueprintCallable, Category = "Scene Panel")
	void ToggleActorVisibility(AActor* Actor);

	UFUNCTION(BlueprintCallable, Category = "Scene Panel")
	void RenameActor(AActor* Actor, const FString& NewName);

	// Observer callbacks for SceneTreeItem events
	UFUNCTION()
	void OnTreeItemSelected(const FSceneTreeItemData& ItemData);

	UFUNCTION()
	void OnTreeItemExpanded(const FSceneTreeItemData& ItemData, bool bExpanded);

	UFUNCTION()
	void OnTreeItemVisibilityChanged(const FSceneTreeItemData& ItemData, bool bVisible);

	UFUNCTION()
	void OnTreeItemRenamed(const FSceneTreeItemData& ItemData, const FString& NewName);

	// Bind to a SceneTreeItem widget
	UFUNCTION(BlueprintCallable, Category = "Scene Panel")
	void BindToTreeItem(USceneTreeItem* TreeItem);

	// Broadcasting events to UI
	DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnSceneTreeRefreshed, const TArray<FSceneTreeItemData>&, SceneItems);
	DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnActorSelectedInScene, AActor*, SelectedActor);

	UPROPERTY(BlueprintAssignable, Category = "Scene Panel Events")
	FOnSceneTreeRefreshed OnSceneTreeRefreshed;

	UPROPERTY(BlueprintAssignable, Category = "Scene Panel Events")
	FOnActorSelectedInScene OnActorSelectedInScene;

protected:
	// Currently selected actor
	UPROPERTY()
	AActor* CurrentSelectedActor;

	// Cache of scene actors
	UPROPERTY()
	TArray<FSceneTreeItemData> CachedSceneData;

	// Helper functions
	void BuildSceneHierarchy(AActor* ParentActor, int32 IndentLevel, TArray<FSceneTreeItemData>& OutItems);
	FSceneTreeItemData CreateItemDataFromActor(AActor* Actor, int32 IndentLevel);
};
