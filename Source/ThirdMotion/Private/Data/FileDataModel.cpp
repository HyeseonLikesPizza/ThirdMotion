// Fill out your copyright notice in the Description page of Project Settings.

#include "Data/FileDataModel.h"

UFileDataModel::UFileDataModel()
{
	bHasUnsavedChanges = false;
	CurrentWorldPath = TEXT("");
	MaxRecentFiles = 10;
}

void UFileDataModel::AddRecentFile(const FString& FilePath)
{
	if (FilePath.IsEmpty())
	{
		return;
	}

	// Remove if already exists
	RecentFiles.Remove(FilePath);

	// Add to front
	RecentFiles.Insert(FilePath, 0);

	// Limit to max size
	if (RecentFiles.Num() > MaxRecentFiles)
	{
		RecentFiles.SetNum(MaxRecentFiles);
	}

	// Save to config
	SaveRecentFilesToConfig();

	// Notify observers
	OnRecentFilesChanged.Broadcast(RecentFiles);
	OnDataChanged.Broadcast();

	UE_LOG(LogTemp, Log, TEXT("FileDataModel: Added recent file '%s'"), *FilePath);
}

void UFileDataModel::ClearRecentFiles()
{
	RecentFiles.Empty();
	SaveRecentFilesToConfig();
	OnRecentFilesChanged.Broadcast(RecentFiles);
	OnDataChanged.Broadcast();

	UE_LOG(LogTemp, Log, TEXT("FileDataModel: Cleared recent files"));
}

void UFileDataModel::SetCurrentWorldPath(const FString& Path)
{
	if (CurrentWorldPath != Path)
	{
		CurrentWorldPath = Path;
		OnCurrentWorldChanged.Broadcast(CurrentWorldPath);
		OnDataChanged.Broadcast();

		UE_LOG(LogTemp, Log, TEXT("FileDataModel: Current world path set to '%s'"), *Path);
	}
}

void UFileDataModel::MarkDirty()
{
	if (!bHasUnsavedChanges)
	{
		bHasUnsavedChanges = true;
		OnDirtyStateChanged.Broadcast(true);
		OnDataChanged.Broadcast();

		UE_LOG(LogTemp, Log, TEXT("FileDataModel: Marked as dirty (unsaved changes)"));
	}
}

void UFileDataModel::MarkClean()
{
	if (bHasUnsavedChanges)
	{
		bHasUnsavedChanges = false;
		OnDirtyStateChanged.Broadcast(false);
		OnDataChanged.Broadcast();

		UE_LOG(LogTemp, Log, TEXT("FileDataModel: Marked as clean (no unsaved changes)"));
	}
}

void UFileDataModel::LoadRecentFilesFromConfig()
{
	if (GConfig)
	{
		TArray<FString> LoadedFiles;
		GConfig->GetArray(TEXT("FileHistory"), TEXT("RecentFiles"), LoadedFiles, GGameUserSettingsIni);

		RecentFiles = LoadedFiles;
		OnRecentFilesChanged.Broadcast(RecentFiles);

		UE_LOG(LogTemp, Log, TEXT("FileDataModel: Loaded %d recent files from config"), RecentFiles.Num());
	}
}

void UFileDataModel::SaveRecentFilesToConfig()
{
	if (GConfig)
	{
		GConfig->SetArray(TEXT("FileHistory"), TEXT("RecentFiles"), RecentFiles, GGameUserSettingsIni);
		GConfig->Flush(false, GGameUserSettingsIni);

		UE_LOG(LogTemp, Log, TEXT("FileDataModel: Saved %d recent files to config"), RecentFiles.Num());
	}
}
