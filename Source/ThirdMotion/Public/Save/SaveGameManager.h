// SaveGameManager.h - Save/Load management with Memento Pattern
// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "Data/ActorSaveData.h"
#include "Save/ThirdMotionSaveGame.h"
#include "SaveGameManager.generated.h"

class AActor;

/**
 * SaveGameManager - Manages Save/Load operations using Memento Pattern
 *
 * Memento Pattern:
 * - Originator: Actors in the scene (create state snapshots)
 * - Memento: FActorSaveData, FSceneSaveData (state snapshots)
 * - Caretaker: SaveGameManager (manages mementos)
 */
UCLASS()
class THIRDMOTION_API USaveGameManager : public UGameInstanceSubsystem
{
	GENERATED_BODY()

public:
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;
	virtual void Deinitialize() override;

	// ============================================================================
	// Save Operations (Create Mementos)
	// ============================================================================

	// Save current scene to default slot
	UFUNCTION(BlueprintCallable, Category = "Save Game")
	bool SaveScene(const FString& SceneName = TEXT("AutoSave"));

	// Save As - specify custom slot name
	UFUNCTION(BlueprintCallable, Category = "Save Game")
	bool SaveSceneAs(const FString& SlotName, const FString& SceneName);

	// Quick save to last used slot
	UFUNCTION(BlueprintCallable, Category = "Save Game")
	bool QuickSave();

	// ============================================================================
	// Load Operations (Restore from Mementos)
	// ============================================================================

	// Load scene from default slot
	UFUNCTION(BlueprintCallable, Category = "Save Game")
	bool LoadScene(const FString& SlotName = TEXT("AutoSave"));

	// Quick load from last used slot
	UFUNCTION(BlueprintCallable, Category = "Save Game")
	bool QuickLoad();

	// ============================================================================
	// Memento Operations
	// ============================================================================

	// Create memento (snapshot) of current scene
	UFUNCTION(BlueprintCallable, Category = "Save Game")
	FSceneSaveData CreateSceneMemento(const FString& SceneName);

	// Restore scene from memento
	UFUNCTION(BlueprintCallable, Category = "Save Game")
	bool RestoreFromMemento(const FSceneSaveData& Memento);

	// Create memento for single actor
	UFUNCTION(BlueprintCallable, Category = "Save Game")
	FActorSaveData CreateActorMemento(AActor* Actor);

	// Restore single actor from memento
	UFUNCTION(BlueprintCallable, Category = "Save Game")
	AActor* RestoreActorFromMemento(const FActorSaveData& Memento, UWorld* World);

	// ============================================================================
	// Save Slot Management
	// ============================================================================

	// Get list of all save slots
	UFUNCTION(BlueprintCallable, Category = "Save Game")
	TArray<FString> GetSaveSlots();

	// Check if save slot exists
	UFUNCTION(BlueprintCallable, Category = "Save Game")
	bool DoesSaveExist(const FString& SlotName);

	// Delete save slot
	UFUNCTION(BlueprintCallable, Category = "Save Game")
	bool DeleteSave(const FString& SlotName);

	// Get save metadata (without loading full scene)
	UFUNCTION(BlueprintCallable, Category = "Save Game")
	bool GetSaveMetadata(const FString& SlotName, FSceneSaveData& OutMetadata);

	// ============================================================================
	// Events
	// ============================================================================

	DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnSceneSaved, const FString&, SlotName);
	DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnSceneLoaded, const FString&, SlotName);
	DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnSaveError, const FString&, ErrorMessage, const FString&, SlotName);

	UPROPERTY(BlueprintAssignable, Category = "Save Events")
	FOnSceneSaved OnSceneSaved;

	UPROPERTY(BlueprintAssignable, Category = "Save Events")
	FOnSceneLoaded OnSceneLoaded;

	UPROPERTY(BlueprintAssignable, Category = "Save Events")
	FOnSaveError OnSaveError;

protected:
	// Last used slot for quick save/load
	UPROPERTY()
	FString LastUsedSlot;

	// Current save game object
	UPROPERTY()
	UThirdMotionSaveGame* CurrentSaveGame;

	// Helper functions
	void CaptureSceneActors(FSceneSaveData& OutSceneData);
	void SpawnActorsFromData(const FSceneSaveData& SceneData);
	void ClearCurrentScene();

	FActorSaveData CaptureActorState(AActor* Actor);
	void ApplyActorState(AActor* Actor, const FActorSaveData& ActorData);

	AActor* SpawnActorFromData(const FActorSaveData& ActorData, UWorld* World);

	// Get save game directory
	FString GetSaveDirectory() const;
};
