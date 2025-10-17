// ScenePanelController.h - Scene Panel Widget Controller
// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "UI/WidgetController/BaseWidgetController.h"
#include "UI/Widget/SceneTreeItem.h"
#include "ScenePanelController.generated.h"

class USceneItemData;

// Delegates
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnSceneTreeRefreshed, const TArray<FSceneTreeItemData>&, SceneItems);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnActorSelectedInScene, AActor*, SelectedActor);

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

	// Refresh the scene tree from world actors
	UFUNCTION(BlueprintCallable, Category = "Scene Panel")
	void RefreshSceneTree();

	// Get current scene actors
	UFUNCTION(BlueprintCallable, Category = "Scene Panel")
	TArray<FSceneTreeItemData> GetSceneActors();

	// Select an actor in the scene
	UFUNCTION(BlueprintCallable, Category = "Scene Panel")
	void SelectActor(AActor* Actor);

	// Toggle actor visibility
	UFUNCTION(BlueprintCallable, Category = "Scene Panel")
	void ToggleActorVisibility(AActor* Actor);

	// Rename actor
	UFUNCTION(BlueprintCallable, Category = "Scene Panel")
	void RenameActor(AActor* Actor, const FString& NewName);

	// Bind to tree item events (Observer Pattern)
	UFUNCTION(BlueprintCallable, Category = "Scene Panel")
	void BindToTreeItem(USceneTreeItem* TreeItem);

	// Events
	UPROPERTY(BlueprintAssignable, Category = "Scene Panel Events")
	FOnSceneTreeRefreshed OnSceneTreeRefreshed;

	UPROPERTY(BlueprintAssignable, Category = "Scene Panel Events")
	FOnActorSelectedInScene OnActorSelectedInScene;

protected:
	// Observer Pattern: Event handlers from SceneTreeItem
	UFUNCTION()
	void OnTreeItemSelected(const FSceneTreeItemData& ItemData);

	UFUNCTION()
	void OnTreeItemExpanded(const FSceneTreeItemData& ItemData, bool bExpanded);

	UFUNCTION()
	void OnTreeItemVisibilityChanged(const FSceneTreeItemData& ItemData, bool bVisible);

	UFUNCTION()
	void OnTreeItemRenamed(const FSceneTreeItemData& ItemData, const FString& NewName);

private:
	// Currently selected actor
	UPROPERTY()
	AActor* CurrentSelectedActor;

	// Cached scene data
	TArray<FSceneTreeItemData> CachedSceneData;

	// Build hierarchy recursively
	void BuildSceneHierarchy(AActor* ParentActor, int32 IndentLevel, TArray<FSceneTreeItemData>& OutItems);

	// Create item data from actor
	FSceneTreeItemData CreateItemDataFromActor(AActor* Actor, int32 IndentLevel);
};
