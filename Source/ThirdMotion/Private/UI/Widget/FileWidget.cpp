// Fill out your copyright notice in the Description page of Project Settings.

#include "UI/Widget/FileWidget.h"
#include "UI/WidgetController/FileController.h"
#include "Data/FileDataModel.h"
#include "Components/Button.h"
#include "Components/TextBlock.h"
#include "Components/VerticalBox.h"

void UFileWidget::NativeConstruct()
{
	Super::NativeConstruct();

	// Bind button events
	if (NewSceneButton)
	{
		NewSceneButton->OnClicked.AddDynamic(this, &UFileWidget::OnNewSceneClicked);
	}

	if (OpenButton)
	{
		OpenButton->OnClicked.AddDynamic(this, &UFileWidget::OnOpenClicked);
	}

	if (OpenRecentButton)
	{
		OpenRecentButton->OnClicked.AddDynamic(this, &UFileWidget::OnOpenRecentClicked);
	}

	if (ShowHomeButton)
	{
		ShowHomeButton->OnClicked.AddDynamic(this, &UFileWidget::OnShowHomeClicked);
	}

	if (SaveButton)
	{
		SaveButton->OnClicked.AddDynamic(this, &UFileWidget::OnSaveClicked);
	}

	if (SaveAsButton)
	{
		SaveAsButton->OnClicked.AddDynamic(this, &UFileWidget::OnSaveAsClicked);
	}

	if (MergeButton)
	{
		MergeButton->OnClicked.AddDynamic(this, &UFileWidget::OnMergeClicked);
	}

	if (QuitButton)
	{
		QuitButton->OnClicked.AddDynamic(this, &UFileWidget::OnQuitClicked);
	}

	UE_LOG(LogTemp, Log, TEXT("FileWidget constructed"));
}

void UFileWidget::NativeDestruct()
{
	// Unbind from model events
	if (DataModel)
	{
		DataModel->OnDataChanged.RemoveDynamic(this, &UFileWidget::OnDataChanged);
		DataModel->OnRecentFilesChanged.RemoveDynamic(this, &UFileWidget::OnRecentFilesChanged);
		DataModel->OnCurrentWorldChanged.RemoveDynamic(this, &UFileWidget::OnCurrentWorldChanged);
		DataModel->OnDirtyStateChanged.RemoveDynamic(this, &UFileWidget::OnDirtyStateChanged);
	}

	// Unbind from controller events
	if (FileController)
	{
		FileController->OnError.RemoveDynamic(this, &UFileWidget::OnFileError);
	}

	Super::NativeDestruct();
}

void UFileWidget::SetupMVC(UFileController* InController, UFileDataModel* InDataModel)
{
	if (!InController || !InDataModel)
	{
		UE_LOG(LogTemp, Error, TEXT("FileWidget: Invalid controller or model in Initialize"));
		return;
	}

	FileController = InController;
	DataModel = InDataModel;

	// Bind to model events (Observer Pattern)
	DataModel->OnDataChanged.AddDynamic(this, &UFileWidget::OnDataChanged);
	DataModel->OnRecentFilesChanged.AddDynamic(this, &UFileWidget::OnRecentFilesChanged);
	DataModel->OnCurrentWorldChanged.AddDynamic(this, &UFileWidget::OnCurrentWorldChanged);
	DataModel->OnDirtyStateChanged.AddDynamic(this, &UFileWidget::OnDirtyStateChanged);

	// Bind to controller events
	FileController->OnError.AddDynamic(this, &UFileWidget::OnFileError);

	// Initial UI refresh
	RefreshUI();

	UE_LOG(LogTemp, Log, TEXT("FileWidget MVC setup completed with controller and model"));
}

void UFileWidget::RefreshUI()
{
	if (!DataModel)
	{
		return;
	}

	// Update all UI elements based on model data
	UpdateCurrentFileDisplay(DataModel->GetCurrentWorldPath());
	UpdateUnsavedIndicator(DataModel->HasUnsavedChanges());
	PopulateRecentFilesList(DataModel->GetRecentFiles());

	UE_LOG(LogTemp, Log, TEXT("FileWidget: UI refreshed"));
}

// Button Click Handlers - Delegate to Controller

void UFileWidget::OnNewSceneClicked()
{
	UE_LOG(LogTemp, Log, TEXT("FileWidget: New Scene button clicked"));

	if (FileController)
	{
		FileController->NewScene();
	}
}

void UFileWidget::OnOpenClicked()
{
	UE_LOG(LogTemp, Log, TEXT("FileWidget: Open button clicked"));

	if (FileController)
	{
		FileController->Open();
	}
}

void UFileWidget::OnOpenRecentClicked()
{
	UE_LOG(LogTemp, Log, TEXT("FileWidget: Open Recent button clicked"));

	// This button toggles visibility of recent files list
	if (RecentFilesContainer)
	{
		ESlateVisibility CurrentVisibility = RecentFilesContainer->GetVisibility();
		if (CurrentVisibility == ESlateVisibility::Visible)
		{
			RecentFilesContainer->SetVisibility(ESlateVisibility::Collapsed);
		}
		else
		{
			RecentFilesContainer->SetVisibility(ESlateVisibility::Visible);
		}
	}
}

void UFileWidget::OnShowHomeClicked()
{
	UE_LOG(LogTemp, Log, TEXT("FileWidget: Show Home button clicked"));

	if (FileController)
	{
		FileController->ShowHome();
	}
}

void UFileWidget::OnSaveClicked()
{
	UE_LOG(LogTemp, Log, TEXT("FileWidget: Save button clicked"));

	if (FileController)
	{
		FileController->Save();
	}
}

void UFileWidget::OnSaveAsClicked()
{
	UE_LOG(LogTemp, Log, TEXT("FileWidget: Save As button clicked"));

	if (FileController)
	{
		FileController->SaveAs(TEXT(""));
	}
}

void UFileWidget::OnMergeClicked()
{
	UE_LOG(LogTemp, Log, TEXT("FileWidget: Merge button clicked"));

	if (FileController)
	{
		FileController->Merge();
	}
}

void UFileWidget::OnQuitClicked()
{
	UE_LOG(LogTemp, Log, TEXT("FileWidget: Quit button clicked"));

	if (FileController)
	{
		FileController->Quit();
	}
}

// Model Event Handlers - Update View

void UFileWidget::OnDataChanged()
{
	UE_LOG(LogTemp, Log, TEXT("FileWidget: Data changed, refreshing UI"));
	RefreshUI();
}

void UFileWidget::OnRecentFilesChanged(const TArray<FString>& RecentFiles)
{
	UE_LOG(LogTemp, Log, TEXT("FileWidget: Recent files changed (%d files)"), RecentFiles.Num());
	PopulateRecentFilesList(RecentFiles);
}

void UFileWidget::OnCurrentWorldChanged(const FString& WorldPath)
{
	UE_LOG(LogTemp, Log, TEXT("FileWidget: Current world changed to '%s'"), *WorldPath);
	UpdateCurrentFileDisplay(WorldPath);
}

void UFileWidget::OnDirtyStateChanged(bool bIsDirty)
{
	UE_LOG(LogTemp, Log, TEXT("FileWidget: Dirty state changed to %s"), bIsDirty ? TEXT("dirty") : TEXT("clean"));
	UpdateUnsavedIndicator(bIsDirty);
}

void UFileWidget::OnFileError(const FString& ErrorMessage)
{
	UE_LOG(LogTemp, Error, TEXT("FileWidget: Error - %s"), *ErrorMessage);

	// TODO: Show error popup or notification to user
}

// Helper Functions

void UFileWidget::PopulateRecentFilesList(const TArray<FString>& RecentFiles)
{
	if (!RecentFilesContainer)
	{
		return;
	}

	// Clear existing list
	RecentFilesContainer->ClearChildren();

	// Add each recent file as a button
	for (const FString& FilePath : RecentFiles)
	{
		// Create button for each recent file
		UButton* RecentButton = NewObject<UButton>(this);
		if (RecentButton)
		{
			// Create text block for button content
			UTextBlock* FileText = NewObject<UTextBlock>(this);
			if (FileText)
			{
				FString FileName = FPaths::GetCleanFilename(FilePath);
				FileText->SetText(FText::FromString(FileName));
				RecentButton->AddChild(FileText);
			}

			// Add to container first
			RecentFilesContainer->AddChild(RecentButton);

			// Note: Dynamic creation of buttons with click events is better handled
			// through Blueprint or by creating custom button widgets.
			// For now, buttons are created but click handlers should be set up in Blueprint
		}
	}

	UE_LOG(LogTemp, Log, TEXT("FileWidget: Populated %d recent files"), RecentFiles.Num());
}

void UFileWidget::UpdateCurrentFileDisplay(const FString& FilePath)
{
	if (!CurrentFileText)
	{
		return;
	}

	if (FilePath.IsEmpty())
	{
		CurrentFileText->SetText(FText::FromString(TEXT("No file open")));
	}
	else
	{
		FString FileName = FPaths::GetCleanFilename(FilePath);
		CurrentFileText->SetText(FText::FromString(FileName));
	}
}

void UFileWidget::UpdateUnsavedIndicator(bool bIsDirty)
{
	if (!UnsavedIndicator)
	{
		return;
	}

	if (bIsDirty)
	{
		UnsavedIndicator->SetText(FText::FromString(TEXT("*")));
		UnsavedIndicator->SetVisibility(ESlateVisibility::Visible);
	}
	else
	{
		UnsavedIndicator->SetVisibility(ESlateVisibility::Collapsed);
	}
}
