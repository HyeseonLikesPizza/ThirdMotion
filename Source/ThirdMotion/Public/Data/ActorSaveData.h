// ActorSaveData.h - Serializable actor data structures
// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "ActorSaveData.generated.h"

/**
 * Memento Pattern: Stores actor state for Save/Load
 * This is a snapshot of an actor's state at a point in time
 */
USTRUCT(BlueprintType)
struct FActorSaveData
{
	GENERATED_BODY()

	// Actor identification
	UPROPERTY(BlueprintReadWrite, SaveGame, Category = "Actor Data")
	FString ActorName;

	UPROPERTY(BlueprintReadWrite, SaveGame, Category = "Actor Data")
	FString ActorClass;

	UPROPERTY(BlueprintReadWrite, SaveGame, Category = "Actor Data")
	FGameplayTag PresetTag;

	// Transform data
	UPROPERTY(BlueprintReadWrite, SaveGame, Category = "Actor Data")
	FTransform ActorTransform;

	// Visibility and state
	UPROPERTY(BlueprintReadWrite, SaveGame, Category = "Actor Data")
	bool bIsHidden;

	UPROPERTY(BlueprintReadWrite, SaveGame, Category = "Actor Data")
	bool bIsEnabled;

	// Hierarchy
	UPROPERTY(BlueprintReadWrite, SaveGame, Category = "Actor Data")
	FString ParentActorName;

	UPROPERTY(BlueprintReadWrite, SaveGame, Category = "Actor Data")
	TArray<FString> ChildActorNames;

	// Custom properties (serialized as JSON or key-value pairs)
	UPROPERTY(BlueprintReadWrite, SaveGame, Category = "Actor Data")
	TMap<FString, FString> CustomProperties;

	// Network data
	UPROPERTY(BlueprintReadWrite, SaveGame, Category = "Actor Data")
	bool bIsReplicated;

	FActorSaveData()
		: ActorName(TEXT(""))
		, ActorClass(TEXT(""))
		, ActorTransform(FTransform::Identity)
		, bIsHidden(false)
		, bIsEnabled(true)
		, ParentActorName(TEXT(""))
		, bIsReplicated(false)
	{
	}
};

/**
 * Complete scene state snapshot
 */
USTRUCT(BlueprintType)
struct FSceneSaveData
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadWrite, SaveGame, Category = "Scene Data")
	FString SceneName;

	UPROPERTY(BlueprintReadWrite, SaveGame, Category = "Scene Data")
	FDateTime SaveTime;

	UPROPERTY(BlueprintReadWrite, SaveGame, Category = "Scene Data")
	FString MapName;

	UPROPERTY(BlueprintReadWrite, SaveGame, Category = "Scene Data")
	TArray<FActorSaveData> ActorDataList;

	// Metadata
	UPROPERTY(BlueprintReadWrite, SaveGame, Category = "Scene Data")
	int32 Version;

	UPROPERTY(BlueprintReadWrite, SaveGame, Category = "Scene Data")
	FString Author;

	FSceneSaveData()
		: SceneName(TEXT("Untitled"))
		, SaveTime(FDateTime::Now())
		, MapName(TEXT(""))
		, Version(1)
		, Author(TEXT(""))
	{
	}
};
