// Fill out your copyright notice in the Description page of Project Settings.

#include "UI/WidgetController/FileController.h"
#include "Data/FileDataModel.h"
#include "Save/SaveGameManager.h"
#include "Misc/Paths.h"
#include "HAL/PlatformProcess.h"
#include "Misc/MessageDialog.h"
#include "DesktopPlatformModule.h"
#include "IDesktopPlatform.h"
#include "Framework/Application/SlateApplication.h"

UFileController::UFileController()
{
	DataModel = nullptr;
}

void UFileController::Init()
{
	Super::Init();

	// Create data model if not exists
	if (!DataModel)
	{
		DataModel = NewObject<UFileDataModel>(this);
		DataModel->LoadRecentFilesFromConfig();
	}

	// Note: SaveGameManager will be accessed when needed through GEngine
	// It's initialized as a GameInstance subsystem

	UE_LOG(LogTemp, Log, TEXT("FileController initialized"));
}

void UFileController::SetDataModel(UFileDataModel* InDataModel)
{
	if (InDataModel)
	{
		DataModel = InDataModel;
		UE_LOG(LogTemp, Log, TEXT("FileController: Data model set"));
	}
}

void UFileController::NewScene()
{
	UE_LOG(LogTemp, Log, TEXT("FileController: New Scene requested"));

	// Check for unsaved changes
	if (DataModel && DataModel->HasUnsavedChanges())
	{
		bool bShouldProceed = CheckUnsavedChanges();
		if (!bShouldProceed)
		{
			UE_LOG(LogTemp, Warning, TEXT("FileController: New Scene cancelled by user"));
			return;
		}
	}

	// Create new world/map
	// Note: Actual level creation/loading should be handled by Unreal's level system
	// This just resets the file tracking state

	// Update model
	if (DataModel)
	{
		DataModel->SetCurrentWorldPath(TEXT(""));
		DataModel->MarkClean();
	}

	// Broadcast event
	OnNewSceneRequested.Broadcast();

	UE_LOG(LogTemp, Log, TEXT("FileController: New Scene created"));
}

void UFileController::Open()
{
	UE_LOG(LogTemp, Log, TEXT("FileController: Open File dialog requested"));

	// Open File folder: C:\Users\user\Desktop\Fork\ThirdMotion_share\File
	FString FileFolderPath = GetProjectFileFolderPath();

	// Open file explorer to the File folder
	FPlatformProcess::ExploreFolder(*FileFolderPath);

	UE_LOG(LogTemp, Log, TEXT("FileController: Opened folder '%s'"), *FileFolderPath);
}

void UFileController::OpenRecent(const FString& FilePath)
{
	UE_LOG(LogTemp, Log, TEXT("FileController: Opening recent file '%s'"), *FilePath);

	if (FilePath.IsEmpty())
	{
		OnError.Broadcast(TEXT("Invalid file path"));
		return;
	}

	// Check if file exists
	if (!FPaths::FileExists(FilePath))
	{
		OnError.Broadcast(FString::Printf(TEXT("File not found: %s"), *FilePath));
		return;
	}

	// Check for unsaved changes
	if (DataModel && DataModel->HasUnsavedChanges())
	{
		bool bShouldProceed = CheckUnsavedChanges();
		if (!bShouldProceed)
		{
			return;
		}
	}

	// Load the world/level using SaveGameManager if available
	if (GEngine && GEngine->GameViewport)
	{
		UGameInstance* GameInstance = GEngine->GameViewport->GetGameInstance();
		if (GameInstance)
		{
			USaveGameManager* SGM = GameInstance->GetSubsystem<USaveGameManager>();
			if (SGM)
			{
				FString FileName = FPaths::GetBaseFilename(FilePath);
				SGM->LoadScene(FileName);
			}
		}
	}

	// Update model
	if (DataModel)
	{
		DataModel->SetCurrentWorldPath(FilePath);
		DataModel->AddRecentFile(FilePath);
		DataModel->MarkClean();
	}

	// Broadcast event
	OnFileOpened.Broadcast(FilePath);

	UE_LOG(LogTemp, Log, TEXT("FileController: File opened successfully"));
}

void UFileController::ShowHome()
{
	UE_LOG(LogTemp, Log, TEXT("FileController: Show Home (Epic Games Launcher) requested"));

	// Epic Games Launcher executable path
	FString LauncherPath = GetEpicGamesLauncherPath();

	if (FPaths::FileExists(LauncherPath))
	{
		// Launch Epic Games Launcher
		FPlatformProcess::CreateProc(*LauncherPath, TEXT(""), true, false, false, nullptr, 0, nullptr, nullptr);
		UE_LOG(LogTemp, Log, TEXT("FileController: Epic Games Launcher started"));
	}
	else
	{
		FString ErrorMsg = FString::Printf(TEXT("Epic Games Launcher not found at: %s"), *LauncherPath);
		OnError.Broadcast(ErrorMsg);
		UE_LOG(LogTemp, Error, TEXT("%s"), *ErrorMsg);
	}
}

void UFileController::Save()
{
	UE_LOG(LogTemp, Log, TEXT("FileController: Save requested"));

	if (!DataModel)
	{
		OnError.Broadcast(TEXT("Data model is null"));
		return;
	}

	FString CurrentPath = DataModel->GetCurrentWorldPath();

	// If no current path, prompt Save As
	if (CurrentPath.IsEmpty())
	{
		UE_LOG(LogTemp, Warning, TEXT("FileController: No current file, prompting Save As"));
		SaveAs(TEXT(""));
		return;
	}

	// Save to current path using SaveGameManager if available
	if (GEngine && GEngine->GameViewport)
	{
		UGameInstance* GameInstance = GEngine->GameViewport->GetGameInstance();
		if (GameInstance)
		{
			USaveGameManager* SGM = GameInstance->GetSubsystem<USaveGameManager>();
			if (SGM)
			{
				FString FileName = FPaths::GetBaseFilename(CurrentPath);
				SGM->SaveScene(FileName);
			}
		}
	}

	// Update model
	DataModel->MarkClean();

	// Broadcast event
	OnSaveCompleted.Broadcast();

	UE_LOG(LogTemp, Log, TEXT("FileController: Saved to '%s'"), *CurrentPath);
}

void UFileController::SaveAs(const FString& NewFileName)
{
	UE_LOG(LogTemp, Log, TEXT("FileController: Save As requested with name '%s'"), *NewFileName);

	FString FinalFileName = NewFileName;

	// If no filename provided, show dialog
	if (FinalFileName.IsEmpty())
	{
		// Open file save dialog
		IDesktopPlatform* DesktopPlatform = FDesktopPlatformModule::Get();
		if (DesktopPlatform)
		{
			TArray<FString> OutFiles;
			const void* ParentWindowHandle = FSlateApplication::Get().FindBestParentWindowHandleForDialogs(nullptr);

			bool bSuccess = DesktopPlatform->SaveFileDialog(
				ParentWindowHandle,
				TEXT("Save World As"),
				GetProjectFileFolderPath(),
				TEXT(""),
				TEXT("World Files (*.umap)|*.umap"),
				EFileDialogFlags::None,
				OutFiles
			);

			if (bSuccess && OutFiles.Num() > 0)
			{
				FinalFileName = FPaths::GetBaseFilename(OutFiles[0]);
			}
			else
			{
				UE_LOG(LogTemp, Warning, TEXT("FileController: Save As cancelled"));
				return;
			}
		}
	}

	if (FinalFileName.IsEmpty())
	{
		OnError.Broadcast(TEXT("Invalid file name"));
		return;
	}

	// Save with new name using SaveGameManager if available
	if (GEngine && GEngine->GameViewport)
	{
		UGameInstance* GameInstance = GEngine->GameViewport->GetGameInstance();
		if (GameInstance)
		{
			USaveGameManager* SGM = GameInstance->GetSubsystem<USaveGameManager>();
			if (SGM)
			{
				SGM->SaveSceneAs(FinalFileName, FinalFileName);
			}
		}
	}

	// Build full path
	FString NewPath = FPaths::Combine(GetProjectFileFolderPath(), FinalFileName + TEXT(".umap"));

	// Update model
	if (DataModel)
	{
		DataModel->SetCurrentWorldPath(NewPath);
		DataModel->AddRecentFile(NewPath);
		DataModel->MarkClean();
	}

	// Broadcast event
	OnSaveAsCompleted.Broadcast(NewPath);

	UE_LOG(LogTemp, Log, TEXT("FileController: Saved as '%s'"), *NewPath);
}

void UFileController::Merge()
{
	UE_LOG(LogTemp, Log, TEXT("FileController: Merge (Git Bash) requested"));

	// Launch Git Bash
	FString GitBashPath = TEXT("C:\\Program Files\\Git\\git-bash.exe");

	if (FPaths::FileExists(GitBashPath))
	{
		// Get project directory
		FString ProjectDir = FPaths::ProjectDir();

		// Launch git bash in project directory
		FPlatformProcess::CreateProc(*GitBashPath, TEXT(""), true, false, false, nullptr, 0, *ProjectDir, nullptr);
		UE_LOG(LogTemp, Log, TEXT("FileController: Git Bash launched in '%s'"), *ProjectDir);
	}
	else
	{
		FString ErrorMsg = FString::Printf(TEXT("Git Bash not found at: %s"), *GitBashPath);
		OnError.Broadcast(ErrorMsg);
		UE_LOG(LogTemp, Error, TEXT("%s"), *ErrorMsg);
	}
}

void UFileController::Quit()
{
	UE_LOG(LogTemp, Log, TEXT("FileController: Quit requested"));

	bool bHasUnsaved = false;

	// Check for unsaved changes
	if (DataModel)
	{
		bHasUnsaved = DataModel->HasUnsavedChanges();
	}

	// Broadcast quit event with unsaved status
	OnQuitRequested.Broadcast(bHasUnsaved);

	if (bHasUnsaved)
	{
		// Show save dialog
		EAppReturnType::Type Response = FMessageDialog::Open(
			EAppMsgType::YesNoCancel,
			FText::FromString(TEXT("You have unsaved changes. Do you want to save before quitting?"))
		);

		if (Response == EAppReturnType::Yes)
		{
			// Save before quitting
			Save();
			FPlatformMisc::RequestExit(false);
		}
		else if (Response == EAppReturnType::No)
		{
			// Quit without saving
			FPlatformMisc::RequestExit(false);
		}
		else
		{
			// Cancel quit
			UE_LOG(LogTemp, Log, TEXT("FileController: Quit cancelled"));
		}
	}
	else
	{
		// No unsaved changes, quit directly
		FPlatformMisc::RequestExit(false);
	}
}

bool UFileController::CheckUnsavedChanges()
{
	EAppReturnType::Type Response = FMessageDialog::Open(
		EAppMsgType::YesNo,
		FText::FromString(TEXT("You have unsaved changes. Do you want to save them?"))
	);

	if (Response == EAppReturnType::Yes)
	{
		Save();
		return true;
	}

	return Response == EAppReturnType::No;
}

void UFileController::ShowSaveDialog()
{
	SaveAs(TEXT(""));
}

FString UFileController::GetProjectFileFolderPath() const
{
	return FPaths::ProjectDir() / TEXT("Data/ThirdMotion_share/File");
}

FString UFileController::GetEpicGamesLauncherPath() const
{
	// Default Epic Games Launcher path
	return TEXT("C:\\Program Files (x86)\\Epic Games\\Launcher\\Portal\\Binaries\\Win64\\EpicGamesLauncher.exe");
}
