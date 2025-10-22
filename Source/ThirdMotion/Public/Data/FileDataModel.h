// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "FileDataModel.generated.h"

/**
 * Model: FileDataModel
 * Manages file-related data including recent files list and current world state
 */
UCLASS()
class THIRDMOTION_API UFileDataModel : public UObject
{
	GENERATED_BODY()

public:
	UFileDataModel();

	// Recent files list
	UPROPERTY()
	TArray<FString> RecentFiles;

	// Current world file path
	UPROPERTY()
	FString CurrentWorldPath;

	// Dirty flag - indicates unsaved changes
	UPROPERTY()
	bool bHasUnsavedChanges;

	// Max recent files to store
	UPROPERTY(EditDefaultsOnly, Category = "File Settings")
	int32 MaxRecentFiles = 10;

	// Add a file to recent files list
	UFUNCTION()
	void AddRecentFile(const FString& FilePath);

	// Get recent files
	UFUNCTION()
	const TArray<FString>& GetRecentFiles() const { return RecentFiles; }

	// Clear recent files
	UFUNCTION()
	void ClearRecentFiles();

	// Set current world path
	UFUNCTION()
	void SetCurrentWorldPath(const FString& Path);

	// Get current world path
	UFUNCTION()
	FString GetCurrentWorldPath() const { return CurrentWorldPath; }

	// Mark as dirty (unsaved changes)
	UFUNCTION()
	void MarkDirty();

	// Mark as clean (no unsaved changes)
	UFUNCTION()
	void MarkClean();

	// Check if has unsaved changes
	UFUNCTION()
	bool HasUnsavedChanges() const { return bHasUnsavedChanges; }

	// Load recent files from config
	UFUNCTION()
	void LoadRecentFilesFromConfig();

	// Save recent files to config
	UFUNCTION()
	void SaveRecentFilesToConfig();

	// Events - Observer Pattern
	DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnDataChanged);
	DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnRecentFilesChanged, const TArray<FString>&, RecentFiles);
	DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnCurrentWorldChanged, const FString&, WorldPath);
	DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnDirtyStateChanged, bool, bIsDirty);

	UPROPERTY(BlueprintAssignable, Category = "File Events")
	FOnDataChanged OnDataChanged;

	UPROPERTY(BlueprintAssignable, Category = "File Events")
	FOnRecentFilesChanged OnRecentFilesChanged;

	UPROPERTY(BlueprintAssignable, Category = "File Events")
	FOnCurrentWorldChanged OnCurrentWorldChanged;

	UPROPERTY(BlueprintAssignable, Category = "File Events")
	FOnDirtyStateChanged OnDirtyStateChanged;
};
