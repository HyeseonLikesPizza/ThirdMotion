// SaveGameManager.cpp
// Copyright Epic Games, Inc. All Rights Reserved.

#include "Save/SaveGameManager.h"
#include "Kismet/GameplayStatics.h"
#include "Engine/World.h"
#include "EngineUtils.h"
#include "GameFramework/Actor.h"
#include "Edit/SceneManager.h"
#include "HAL/PlatformFileManager.h"
#include "Misc/Paths.h"

void USaveGameManager::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);

	LastUsedSlot = TEXT("AutoSave");
	CurrentSaveGame = nullptr;

	UE_LOG(LogTemp, Log, TEXT("SaveGameManager initialized"));
}

void USaveGameManager::Deinitialize()
{
	Super::Deinitialize();

	UE_LOG(LogTemp, Log, TEXT("SaveGameManager deinitialized"));
}

// ============================================================================
// Save Operations
// ============================================================================

bool USaveGameManager::SaveScene(const FString& SceneName)
{
	return SaveSceneAs(TEXT("AutoSave"), SceneName);
}

bool USaveGameManager::SaveSceneAs(const FString& SlotName, const FString& SceneName)
{
	UE_LOG(LogTemp, Log, TEXT("SaveGameManager: Saving scene '%s' to slot '%s'"), *SceneName, *SlotName);

	// Create memento of current scene
	FSceneSaveData SceneMemento = CreateSceneMemento(SceneName);

	// Create save game object
	UThirdMotionSaveGame* SaveGameInstance = Cast<UThirdMotionSaveGame>(
		UGameplayStatics::CreateSaveGameObject(UThirdMotionSaveGame::StaticClass())
	);

	if (!SaveGameInstance)
	{
		UE_LOG(LogTemp, Error, TEXT("SaveGameManager: Failed to create save game object"));
		OnSaveError.Broadcast(TEXT("Failed to create save game object"), SlotName);
		return false;
	}

	// Set data
	SaveGameInstance->SetSceneData(SceneMemento);
	SaveGameInstance->SaveSlotName = SlotName;
	SaveGameInstance->UserIndex = 0;

	// Save to disk
	bool bSuccess = UGameplayStatics::SaveGameToSlot(SaveGameInstance, SlotName, 0);

	if (bSuccess)
	{
		LastUsedSlot = SlotName;
		CurrentSaveGame = SaveGameInstance;
		OnSceneSaved.Broadcast(SlotName);
		UE_LOG(LogTemp, Log, TEXT("SaveGameManager: Scene saved successfully - %d actors"),
			SceneMemento.ActorDataList.Num());
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("SaveGameManager: Failed to save scene"));
		OnSaveError.Broadcast(TEXT("Failed to write save file"), SlotName);
	}

	return bSuccess;
}

bool USaveGameManager::QuickSave()
{
	return SaveSceneAs(LastUsedSlot, TEXT("QuickSave"));
}

// ============================================================================
// Load Operations
// ============================================================================

bool USaveGameManager::LoadScene(const FString& SlotName)
{
	UE_LOG(LogTemp, Log, TEXT("SaveGameManager: Loading scene from slot '%s'"), *SlotName);

	if (!DoesSaveExist(SlotName))
	{
		UE_LOG(LogTemp, Warning, TEXT("SaveGameManager: Save slot '%s' does not exist"), *SlotName);
		OnSaveError.Broadcast(TEXT("Save file does not exist"), SlotName);
		return false;
	}

	// Load save game object
	UThirdMotionSaveGame* LoadedGame = Cast<UThirdMotionSaveGame>(
		UGameplayStatics::LoadGameFromSlot(SlotName, 0)
	);

	if (!LoadedGame)
	{
		UE_LOG(LogTemp, Error, TEXT("SaveGameManager: Failed to load save game"));
		OnSaveError.Broadcast(TEXT("Failed to load save file"), SlotName);
		return false;
	}

	// Restore from memento
	FSceneSaveData SceneMemento = LoadedGame->GetSceneData();
	bool bSuccess = RestoreFromMemento(SceneMemento);

	if (bSuccess)
	{
		LastUsedSlot = SlotName;
		CurrentSaveGame = LoadedGame;
		OnSceneLoaded.Broadcast(SlotName);
		UE_LOG(LogTemp, Log, TEXT("SaveGameManager: Scene loaded successfully - %d actors spawned"),
			SceneMemento.ActorDataList.Num());
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("SaveGameManager: Failed to restore scene from memento"));
		OnSaveError.Broadcast(TEXT("Failed to restore scene"), SlotName);
	}

	return bSuccess;
}

bool USaveGameManager::QuickLoad()
{
	return LoadScene(LastUsedSlot);
}

// ============================================================================
// Memento Operations
// ============================================================================

FSceneSaveData USaveGameManager::CreateSceneMemento(const FString& SceneName)
{
	FSceneSaveData Memento;
	Memento.SceneName = SceneName;
	Memento.SaveTime = FDateTime::Now();

	UWorld* World = GetWorld();
	if (World)
	{
		Memento.MapName = World->GetMapName();
		CaptureSceneActors(Memento);
	}

	UE_LOG(LogTemp, Log, TEXT("SaveGameManager: Created scene memento - %d actors captured"),
		Memento.ActorDataList.Num());

	return Memento;
}

bool USaveGameManager::RestoreFromMemento(const FSceneSaveData& Memento)
{
	UWorld* World = GetWorld();
	if (!World)
	{
		UE_LOG(LogTemp, Error, TEXT("SaveGameManager: World is null"));
		return false;
	}

	// Clear current scene
	ClearCurrentScene();

	// Spawn actors from memento
	SpawnActorsFromData(Memento);

	return true;
}

FActorSaveData USaveGameManager::CreateActorMemento(AActor* Actor)
{
	if (!Actor)
	{
		return FActorSaveData();
	}

	return CaptureActorState(Actor);
}

AActor* USaveGameManager::RestoreActorFromMemento(const FActorSaveData& Memento, UWorld* World)
{
	if (!World)
	{
		UE_LOG(LogTemp, Error, TEXT("SaveGameManager: World is null"));
		return nullptr;
	}

	return SpawnActorFromData(Memento, World);
}

// ============================================================================
// Save Slot Management
// ============================================================================

TArray<FString> USaveGameManager::GetSaveSlots()
{
	TArray<FString> SaveSlots;

	// Get save directory
	FString SaveDir = FPaths::ProjectSavedDir() / TEXT("SaveGames");

	IPlatformFile& PlatformFile = FPlatformFileManager::Get().GetPlatformFile();

	// Find all .sav files
	PlatformFile.IterateDirectory(*SaveDir, [&SaveSlots](const TCHAR* FilenameOrDirectory, bool bIsDirectory) -> bool
	{
		if (!bIsDirectory)
		{
			FString Filename = FPaths::GetBaseFilename(FilenameOrDirectory);
			if (Filename.EndsWith(TEXT(".sav")))
			{
				Filename.RemoveFromEnd(TEXT(".sav"));
				SaveSlots.Add(Filename);
			}
		}
		return true;
	});

	return SaveSlots;
}

bool USaveGameManager::DoesSaveExist(const FString& SlotName)
{
	return UGameplayStatics::DoesSaveGameExist(SlotName, 0);
}

bool USaveGameManager::DeleteSave(const FString& SlotName)
{
	bool bSuccess = UGameplayStatics::DeleteGameInSlot(SlotName, 0);

	if (bSuccess)
	{
		UE_LOG(LogTemp, Log, TEXT("SaveGameManager: Deleted save slot '%s'"), *SlotName);
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("SaveGameManager: Failed to delete save slot '%s'"), *SlotName);
	}

	return bSuccess;
}

bool USaveGameManager::GetSaveMetadata(const FString& SlotName, FSceneSaveData& OutMetadata)
{
	if (!DoesSaveExist(SlotName))
	{
		return false;
	}

	UThirdMotionSaveGame* SaveGame = Cast<UThirdMotionSaveGame>(
		UGameplayStatics::LoadGameFromSlot(SlotName, 0)
	);

	if (SaveGame)
	{
		OutMetadata = SaveGame->GetSceneData();
		return true;
	}

	return false;
}

// ============================================================================
// Helper Functions
// ============================================================================

void USaveGameManager::CaptureSceneActors(FSceneSaveData& OutSceneData)
{
	UWorld* World = GetWorld();
	if (!World) return;

	OutSceneData.ActorDataList.Empty();

	// Iterate through all actors
	for (TActorIterator<AActor> It(World); It; ++It)
	{
		AActor* Actor = *It;

		// Skip system actors and world settings
		if (!Actor || Actor->IsA(AWorldSettings::StaticClass()))
		{
			continue;
		}

		// Only save spawned actors (not placed in editor)
		// You can adjust this logic based on your needs
		if (Actor->HasAnyFlags(RF_Transient))
		{
			continue;
		}

		FActorSaveData ActorData = CaptureActorState(Actor);
		OutSceneData.ActorDataList.Add(ActorData);
	}

	UE_LOG(LogTemp, Log, TEXT("SaveGameManager: Captured %d actors"), OutSceneData.ActorDataList.Num());
}

void USaveGameManager::SpawnActorsFromData(const FSceneSaveData& SceneData)
{
	UWorld* World = GetWorld();
	if (!World) return;

	for (const FActorSaveData& ActorData : SceneData.ActorDataList)
	{
		SpawnActorFromData(ActorData, World);
	}

	UE_LOG(LogTemp, Log, TEXT("SaveGameManager: Spawned %d actors"), SceneData.ActorDataList.Num());
}

void USaveGameManager::ClearCurrentScene()
{
	UWorld* World = GetWorld();
	if (!World) return;

	TArray<AActor*> ActorsToDestroy;

	// Collect actors to destroy
	for (TActorIterator<AActor> It(World); It; ++It)
	{
		AActor* Actor = *It;

		if (!Actor || Actor->IsA(AWorldSettings::StaticClass()) || Actor->HasAnyFlags(RF_Transient))
		{
			continue;
		}

		ActorsToDestroy.Add(Actor);
	}

	// Destroy actors
	for (AActor* Actor : ActorsToDestroy)
	{
		Actor->Destroy();
	}

	UE_LOG(LogTemp, Log, TEXT("SaveGameManager: Cleared %d actors from scene"), ActorsToDestroy.Num());
}

FActorSaveData USaveGameManager::CaptureActorState(AActor* Actor)
{
	FActorSaveData Data;

	if (!Actor) return Data;

	Data.ActorName = Actor->GetName();
	Data.ActorClass = Actor->GetClass()->GetPathName();
	Data.ActorTransform = Actor->GetActorTransform();
	Data.bIsHidden = Actor->IsHidden();
	Data.bIsEnabled = !Actor->IsActorBeingDestroyed();

	// Hierarchy
	AActor* ParentActor = Actor->GetAttachParentActor();
	if (ParentActor)
	{
		Data.ParentActorName = ParentActor->GetName();
	}

	TArray<AActor*> AttachedActors;
	Actor->GetAttachedActors(AttachedActors);
	for (AActor* Child : AttachedActors)
	{
		Data.ChildActorNames.Add(Child->GetName());
	}

	// Network
	Data.bIsReplicated = Actor->GetIsReplicated();

	return Data;
}

void USaveGameManager::ApplyActorState(AActor* Actor, const FActorSaveData& ActorData)
{
	if (!Actor) return;

	Actor->SetActorTransform(ActorData.ActorTransform);
	Actor->SetActorHiddenInGame(ActorData.bIsHidden);
}

AActor* USaveGameManager::SpawnActorFromData(const FActorSaveData& ActorData, UWorld* World)
{
	if (!World) return nullptr;

	// Load actor class
	UClass* ActorClass = LoadObject<UClass>(nullptr, *ActorData.ActorClass);
	if (!ActorClass)
	{
		UE_LOG(LogTemp, Warning, TEXT("SaveGameManager: Failed to load actor class: %s"), *ActorData.ActorClass);
		return nullptr;
	}

	// Spawn actor
	FActorSpawnParameters SpawnParams;
	SpawnParams.Name = FName(*ActorData.ActorName);
	SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

	AActor* SpawnedActor = World->SpawnActor<AActor>(ActorClass, ActorData.ActorTransform, SpawnParams);

	if (SpawnedActor)
	{
		ApplyActorState(SpawnedActor, ActorData);
		UE_LOG(LogTemp, Log, TEXT("SaveGameManager: Spawned actor '%s'"), *ActorData.ActorName);
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("SaveGameManager: Failed to spawn actor '%s'"), *ActorData.ActorName);
	}

	return SpawnedActor;
}

FString USaveGameManager::GetSaveDirectory() const
{
	return FPaths::ProjectSavedDir() / TEXT("SaveGames");
}
