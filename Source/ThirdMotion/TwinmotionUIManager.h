// TwinmotionUIManager.h - UI State Management for Twinmotion-style Interface
// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "Blueprint/UserWidget.h"
#include "TwinmotionUIManager.generated.h"

class UWBPTwinmotionUI;

UENUM(BlueprintType)
enum class ETwinmotionPanelType : uint8
{
	Library UMETA(DisplayName = "Library"),
	Scene UMETA(DisplayName = "Scene"),
	Properties UMETA(DisplayName = "Properties"),
	Thumbnail UMETA(DisplayName = "Thumbnail")
};

UENUM(BlueprintType)
enum class ETwinmotionBottomTab : uint8
{
	Import UMETA(DisplayName = "Import"),
	Modeling UMETA(DisplayName = "Modeling"),
	Media UMETA(DisplayName = "Media"),
	Export UMETA(DisplayName = "Export")
};

/**
 * Structure to hold thumbnail data
 */
USTRUCT(BlueprintType)
struct FThumbnailData
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Thumbnail")
	FString Name;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Thumbnail")
	UTexture2D* Thumbnail;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Thumbnail")
	FDateTime CaptureTime;

	FThumbnailData()
		: Name(TEXT("Untitled"))
		, Thumbnail(nullptr)
		, CaptureTime(FDateTime::Now())
	{
	}
};

/**
 * Twinmotion UI Manager - Manages the entire UI state and data
 * Implemented as a Game Instance Subsystem for persistent state across levels
 */
UCLASS()
class THIRDMOTION_API UTwinmotionUIManager : public UGameInstanceSubsystem
{
	GENERATED_BODY()

public:
	// Subsystem Initialization
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;
	virtual void Deinitialize() override;

	// UI Widget Management
	UFUNCTION(BlueprintCallable, Category = "Twinmotion UI")
	void CreateMainUI(TSubclassOf<UUserWidget> MainUIClass);

	UFUNCTION(BlueprintCallable, Category = "Twinmotion UI")
	UUserWidget* GetMainUI() const { return MainUIWidget; }

	// Panel Visibility Control
	UFUNCTION(BlueprintCallable, Category = "Twinmotion UI")
	void SetPanelVisibility(ETwinmotionPanelType PanelType, bool bVisible);

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Twinmotion UI")
	bool IsPanelVisible(ETwinmotionPanelType PanelType) const;

	// Bottom Tab Management
	UFUNCTION(BlueprintCallable, Category = "Twinmotion UI")
	void SetActiveBottomTab(ETwinmotionBottomTab Tab);

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Twinmotion UI")
	ETwinmotionBottomTab GetActiveBottomTab() const { return CurrentBottomTab; }

	// Thumbnail Management
	UFUNCTION(BlueprintCallable, Category = "Twinmotion UI")
	void AddThumbnail(const FThumbnailData& ThumbnailData);

	UFUNCTION(BlueprintCallable, Category = "Twinmotion UI")
	void RemoveThumbnail(int32 Index);

	UFUNCTION(BlueprintCallable, Category = "Twinmotion UI")
	void ClearAllThumbnails();

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Twinmotion UI")
	const TArray<FThumbnailData>& GetThumbnails() const { return Thumbnails; }

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Twinmotion UI")
	int32 GetThumbnailCount() const { return Thumbnails.Num(); }

	// Library Asset Management
	UFUNCTION(BlueprintCallable, Category = "Twinmotion UI")
	void RefreshLibraryAssets();

	UFUNCTION(BlueprintCallable, Category = "Twinmotion UI")
	void FilterLibraryAssets(const FString& SearchTerm);

	// Scene Hierarchy Management
	UFUNCTION(BlueprintCallable, Category = "Twinmotion UI")
	void RefreshSceneHierarchy();

	UFUNCTION(BlueprintCallable, Category = "Twinmotion UI")
	void SelectSceneObject(AActor* Actor);

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Twinmotion UI")
	AActor* GetSelectedSceneObject() const { return SelectedActor; }

	// Properties Panel
	UFUNCTION(BlueprintCallable, Category = "Twinmotion UI")
	void UpdatePropertiesPanel(AActor* Actor);

	// Delegates for UI updates
	DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnPanelVisibilityChanged, ETwinmotionPanelType, PanelType, bool, bVisible);
	DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnBottomTabChanged, ETwinmotionBottomTab, NewTab);
	DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnThumbnailsUpdated);
	DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnSceneObjectSelected, AActor*, SelectedObject);

	UPROPERTY(BlueprintAssignable, Category = "Twinmotion UI Events")
	FOnPanelVisibilityChanged OnPanelVisibilityChanged;

	UPROPERTY(BlueprintAssignable, Category = "Twinmotion UI Events")
	FOnBottomTabChanged OnBottomTabChanged;

	UPROPERTY(BlueprintAssignable, Category = "Twinmotion UI Events")
	FOnThumbnailsUpdated OnThumbnailsUpdated;

	UPROPERTY(BlueprintAssignable, Category = "Twinmotion UI Events")
	FOnSceneObjectSelected OnSceneObjectSelected;

protected:
	// Main UI Widget Reference
	UPROPERTY()
	UUserWidget* MainUIWidget;

	// Panel Visibility States
	UPROPERTY()
	TMap<ETwinmotionPanelType, bool> PanelVisibilityStates;

	// Current Bottom Tab
	UPROPERTY()
	ETwinmotionBottomTab CurrentBottomTab;

	// Thumbnail Collection
	UPROPERTY()
	TArray<FThumbnailData> Thumbnails;

	// Selected Actor in Scene
	UPROPERTY()
	AActor* SelectedActor;

	// Library Filter
	UPROPERTY()
	FString CurrentLibraryFilter;

private:
	void InitializeDefaultStates();
};
