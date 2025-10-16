// ThirdMotionSaveGame.h - Save game object
// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/SaveGame.h"
#include "Data/ActorSaveData.h"
#include "ThirdMotionSaveGame.generated.h"

/**
 * SaveGame object that stores scene state
 * Uses Unreal's built-in serialization system
 */
UCLASS()
class THIRDMOTION_API UThirdMotionSaveGame : public USaveGame
{
	GENERATED_BODY()

public:
	UThirdMotionSaveGame();

	// Scene data (Memento)
	UPROPERTY(VisibleAnywhere, SaveGame, Category = "Save Data")
	FSceneSaveData SceneData;

	// Save metadata
	UPROPERTY(VisibleAnywhere, SaveGame, Category = "Save Data")
	FString SaveSlotName;

	UPROPERTY(VisibleAnywhere, SaveGame, Category = "Save Data")
	int32 UserIndex;

	// Helper functions
	void SetSceneData(const FSceneSaveData& InSceneData);
	FSceneSaveData GetSceneData() const;

	void AddActorData(const FActorSaveData& ActorData);
	void ClearActorData();

	int32 GetActorCount() const;
};
