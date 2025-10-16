// ScenePanelController.cpp
// Copyright Epic Games, Inc. All Rights Reserved.

#include "UI/WidgetController/ScenePanelController.h"
#include "Engine/World.h"
#include "EngineUtils.h"
#include "GameFramework/Actor.h"

void UScenePanelController::Init()
{
	Super::Init();

	CurrentSelectedActor = nullptr;
	CachedSceneData.Empty();

	UE_LOG(LogTemp, Log, TEXT("ScenePanelController initialized"));
}

void UScenePanelController::RefreshSceneTree()
{
	CachedSceneData.Empty();

	UWorld* World = GetWorld();
	if (!World)
	{
		UE_LOG(LogTemp, Warning, TEXT("ScenePanelController: World is null"));
		return;
	}

	// Get all actors in the world
	for (TActorIterator<AActor> It(World); It; ++It)
	{
		AActor* Actor = *It;
		if (Actor && !Actor->IsHidden())
		{
			// Only add root actors (actors without parent attachment)
			if (!Actor->GetAttachParentActor())
			{
				BuildSceneHierarchy(Actor, 0, CachedSceneData);
			}
		}
	}

	// Broadcast refresh event
	OnSceneTreeRefreshed.Broadcast(CachedSceneData);

	UE_LOG(LogTemp, Log, TEXT("Scene tree refreshed: %d items"), CachedSceneData.Num());
}

TArray<FSceneTreeItemData> UScenePanelController::GetSceneActors()
{
	return CachedSceneData;
}

void UScenePanelController::SelectActor(AActor* Actor)
{
	if (!Actor) return;

	CurrentSelectedActor = Actor;
	OnActorSelectedInScene.Broadcast(Actor);

	UE_LOG(LogTemp, Log, TEXT("Actor selected: %s"), *Actor->GetName());
}

void UScenePanelController::ToggleActorVisibility(AActor* Actor)
{
	if (!Actor) return;

	bool bNewVisibility = !Actor->IsHidden();
	Actor->SetActorHiddenInGame(bNewVisibility);

	UE_LOG(LogTemp, Log, TEXT("Actor visibility toggled: %s -> %s"),
		*Actor->GetName(), bNewVisibility ? TEXT("Hidden") : TEXT("Visible"));
}

void UScenePanelController::RenameActor(AActor* Actor, const FString& NewName)
{
	if (!Actor) return;

	Actor->Rename(*NewName);

	UE_LOG(LogTemp, Log, TEXT("Actor renamed to: %s"), *NewName);
}

void UScenePanelController::OnTreeItemSelected(const FSceneTreeItemData& ItemData)
{
	if (ItemData.Actor)
	{
		SelectActor(ItemData.Actor);
	}
}

void UScenePanelController::OnTreeItemExpanded(const FSceneTreeItemData& ItemData, bool bExpanded)
{
	UE_LOG(LogTemp, Log, TEXT("Tree item %s: %s"),
		*ItemData.Name, bExpanded ? TEXT("Expanded") : TEXT("Collapsed"));
}

void UScenePanelController::OnTreeItemVisibilityChanged(const FSceneTreeItemData& ItemData, bool bVisible)
{
	if (ItemData.Actor)
	{
		ToggleActorVisibility(ItemData.Actor);
	}
}

void UScenePanelController::OnTreeItemRenamed(const FSceneTreeItemData& ItemData, const FString& NewName)
{
	if (ItemData.Actor)
	{
		RenameActor(ItemData.Actor, NewName);
		RefreshSceneTree();
	}
}

void UScenePanelController::BindToTreeItem(USceneTreeItem* TreeItem)
{
	if (!TreeItem) return;

	// Observer Pattern: Subscribe to all tree item events
	TreeItem->OnTreeItemSelected.AddDynamic(this, &UScenePanelController::OnTreeItemSelected);
	TreeItem->OnTreeItemExpanded.AddDynamic(this, &UScenePanelController::OnTreeItemExpanded);
	TreeItem->OnTreeItemVisibilityChanged.AddDynamic(this, &UScenePanelController::OnTreeItemVisibilityChanged);
	TreeItem->OnTreeItemRenamed.AddDynamic(this, &UScenePanelController::OnTreeItemRenamed);

	UE_LOG(LogTemp, Log, TEXT("Bound to tree item"));
}

void UScenePanelController::BuildSceneHierarchy(AActor* ParentActor, int32 IndentLevel, TArray<FSceneTreeItemData>& OutItems)
{
	if (!ParentActor) return;

	// Add this actor
	FSceneTreeItemData ItemData = CreateItemDataFromActor(ParentActor, IndentLevel);
	OutItems.Add(ItemData);

	// Get all attached children
	TArray<AActor*> AttachedActors;
	ParentActor->GetAttachedActors(AttachedActors);

	if (AttachedActors.Num() > 0)
	{
		// Recursively add children
		for (AActor* ChildActor : AttachedActors)
		{
			if (ChildActor && !ChildActor->IsHidden())
			{
				BuildSceneHierarchy(ChildActor, IndentLevel + 1, OutItems);
			}
		}
	}
}

FSceneTreeItemData UScenePanelController::CreateItemDataFromActor(AActor* Actor, int32 IndentLevel)
{
	FSceneTreeItemData ItemData;

	if (Actor)
	{
		ItemData.Name = Actor->GetActorLabel();
		ItemData.Actor = Actor;
		ItemData.IndentLevel = IndentLevel;
		ItemData.bIsVisible = !Actor->IsHidden();

		// Check if has children
		TArray<AActor*> AttachedActors;
		Actor->GetAttachedActors(AttachedActors);
		ItemData.bHasChildren = AttachedActors.Num() > 0;

		ItemData.bIsExpanded = false;
		ItemData.Icon = nullptr; // Set icon based on actor type if needed
	}

	return ItemData;
}
