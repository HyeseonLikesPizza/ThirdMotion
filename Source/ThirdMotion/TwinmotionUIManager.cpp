// TwinmotionUIManager.cpp - UI State Management for Twinmotion-style Interface
// Copyright Epic Games, Inc. All Rights Reserved.

#include "TwinmotionUIManager.h"
#include "Blueprint/UserWidget.h"
#include "Engine/World.h"
#include "GameFramework/Actor.h"

void UTwinmotionUIManager::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);
	InitializeDefaultStates();
	UE_LOG(LogTemp, Log, TEXT("TwinmotionUIManager initialized"));
}

void UTwinmotionUIManager::Deinitialize()
{
	if (MainUIWidget && MainUIWidget->IsValidLowLevel())
	{
		MainUIWidget->RemoveFromParent();
		MainUIWidget = nullptr;
	}

	Super::Deinitialize();
	UE_LOG(LogTemp, Log, TEXT("TwinmotionUIManager deinitialized"));
}

void UTwinmotionUIManager::InitializeDefaultStates()
{
	// Initialize default panel visibility
	PanelVisibilityStates.Add(ETwinmotionPanelType::Library, true);
	PanelVisibilityStates.Add(ETwinmotionPanelType::Scene, true);
	PanelVisibilityStates.Add(ETwinmotionPanelType::Properties, true);
	PanelVisibilityStates.Add(ETwinmotionPanelType::Thumbnail, true);

	// Set default bottom tab
	CurrentBottomTab = ETwinmotionBottomTab::Import;

	// Initialize empty collections
	Thumbnails.Empty();
	SelectedActor = nullptr;
	CurrentLibraryFilter = TEXT("");
}

void UTwinmotionUIManager::CreateMainUI(TSubclassOf<UUserWidget> MainUIClass)
{
	if (!MainUIClass)
	{
		UE_LOG(LogTemp, Warning, TEXT("TwinmotionUIManager: Invalid MainUIClass provided"));
		return;
	}

	// Remove existing UI if present
	if (MainUIWidget && MainUIWidget->IsValidLowLevel())
	{
		MainUIWidget->RemoveFromParent();
	}

	// Create new UI widget
	UWorld* World = GetWorld();
	if (World)
	{
		MainUIWidget = CreateWidget<UUserWidget>(World, MainUIClass);
		if (MainUIWidget)
		{
			MainUIWidget->AddToViewport();
			UE_LOG(LogTemp, Log, TEXT("TwinmotionUIManager: Main UI created successfully"));
		}
	}
}

void UTwinmotionUIManager::SetPanelVisibility(ETwinmotionPanelType PanelType, bool bVisible)
{
	if (PanelVisibilityStates.Contains(PanelType))
	{
		PanelVisibilityStates[PanelType] = bVisible;
		OnPanelVisibilityChanged.Broadcast(PanelType, bVisible);
		UE_LOG(LogTemp, Log, TEXT("Panel visibility changed: %d -> %s"),
			static_cast<int32>(PanelType), bVisible ? TEXT("Visible") : TEXT("Hidden"));
	}
}

bool UTwinmotionUIManager::IsPanelVisible(ETwinmotionPanelType PanelType) const
{
	if (PanelVisibilityStates.Contains(PanelType))
	{
		return PanelVisibilityStates[PanelType];
	}
	return false;
}

void UTwinmotionUIManager::SetActiveBottomTab(ETwinmotionBottomTab Tab)
{
	if (CurrentBottomTab != Tab)
	{
		CurrentBottomTab = Tab;
		OnBottomTabChanged.Broadcast(Tab);
		UE_LOG(LogTemp, Log, TEXT("Bottom tab changed to: %d"), static_cast<int32>(Tab));
	}
}

void UTwinmotionUIManager::AddThumbnail(const FThumbnailData& ThumbnailData)
{
	Thumbnails.Add(ThumbnailData);
	OnThumbnailsUpdated.Broadcast();
	UE_LOG(LogTemp, Log, TEXT("Thumbnail added: %s (Total: %d)"), *ThumbnailData.Name, Thumbnails.Num());
}

void UTwinmotionUIManager::RemoveThumbnail(int32 Index)
{
	if (Thumbnails.IsValidIndex(Index))
	{
		FString RemovedName = Thumbnails[Index].Name;
		Thumbnails.RemoveAt(Index);
		OnThumbnailsUpdated.Broadcast();
		UE_LOG(LogTemp, Log, TEXT("Thumbnail removed: %s (Remaining: %d)"), *RemovedName, Thumbnails.Num());
	}
}

void UTwinmotionUIManager::ClearAllThumbnails()
{
	int32 Count = Thumbnails.Num();
	Thumbnails.Empty();
	OnThumbnailsUpdated.Broadcast();
	UE_LOG(LogTemp, Log, TEXT("All thumbnails cleared (%d removed)"), Count);
}

void UTwinmotionUIManager::RefreshLibraryAssets()
{
	// This would be implemented to scan content browser assets
	// For now, just log the action
	UE_LOG(LogTemp, Log, TEXT("Library assets refreshed"));
}

void UTwinmotionUIManager::FilterLibraryAssets(const FString& SearchTerm)
{
	CurrentLibraryFilter = SearchTerm;
	UE_LOG(LogTemp, Log, TEXT("Library filter set to: %s"), *SearchTerm);
}

void UTwinmotionUIManager::RefreshSceneHierarchy()
{
	// This would be implemented to rebuild scene tree
	UE_LOG(LogTemp, Log, TEXT("Scene hierarchy refreshed"));
}

void UTwinmotionUIManager::SelectSceneObject(AActor* Actor)
{
	if (SelectedActor != Actor)
	{
		SelectedActor = Actor;
		OnSceneObjectSelected.Broadcast(Actor);

		if (Actor)
		{
			UE_LOG(LogTemp, Log, TEXT("Scene object selected: %s"), *Actor->GetName());
			UpdatePropertiesPanel(Actor);
		}
		else
		{
			UE_LOG(LogTemp, Log, TEXT("Scene object deselected"));
		}
	}
}

void UTwinmotionUIManager::UpdatePropertiesPanel(AActor* Actor)
{
	// This would be implemented to update properties panel with actor details
	if (Actor)
	{
		UE_LOG(LogTemp, Log, TEXT("Properties panel updated for: %s"), *Actor->GetName());
	}
}
