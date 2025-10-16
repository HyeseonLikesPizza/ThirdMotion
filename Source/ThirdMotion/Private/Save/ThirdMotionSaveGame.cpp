// ThirdMotionSaveGame.cpp
// Copyright Epic Games, Inc. All Rights Reserved.

#include "Save/ThirdMotionSaveGame.h"

UThirdMotionSaveGame::UThirdMotionSaveGame()
{
	SaveSlotName = TEXT("DefaultSlot");
	UserIndex = 0;
}

void UThirdMotionSaveGame::SetSceneData(const FSceneSaveData& InSceneData)
{
	SceneData = InSceneData;
	UE_LOG(LogTemp, Log, TEXT("SaveGame: Scene data set - %s (%d actors)"),
		*SceneData.SceneName, SceneData.ActorDataList.Num());
}

FSceneSaveData UThirdMotionSaveGame::GetSceneData() const
{
	return SceneData;
}

void UThirdMotionSaveGame::AddActorData(const FActorSaveData& ActorData)
{
	SceneData.ActorDataList.Add(ActorData);
}

void UThirdMotionSaveGame::ClearActorData()
{
	SceneData.ActorDataList.Empty();
}

int32 UThirdMotionSaveGame::GetActorCount() const
{
	return SceneData.ActorDataList.Num();
}
