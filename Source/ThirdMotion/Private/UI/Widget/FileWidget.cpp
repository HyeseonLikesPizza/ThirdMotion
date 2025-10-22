// Fill out your copyright notice in the Description page of Project Settings.

#include "UI/Widget/FileWidget.h"
#include "UI/WidgetController/FileController.h"
#include "UI/WidgetController/TopBarController.h"
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

}

void UFileWidget::SetTopBarController(class UTopBarController* InTopBarController)
{
	TopBarController = InTopBarController;
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

}


void UFileWidget::OnNewSceneClicked()
{
	if (FileController)
	{
		FileController->NewScene();
	}

	// Close dropdown menu after action
	if (TopBarController)
	{
		TopBarController->CloseFilePanel();
	}
}

void UFileWidget::OnOpenClicked()
{
	if (FileController)
	{
		FileController->Open();
	}

	// Close dropdown menu after action
	if (TopBarController)
	{
		TopBarController->CloseFilePanel();
	}
}

void UFileWidget::OnOpenRecentClicked()
{

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
	if (FileController)
	{
		FileController->ShowHome();
	}

	// Close dropdown menu after action
	if (TopBarController)
	{
		TopBarController->CloseFilePanel();
	}
}

void UFileWidget::OnSaveClicked()
{
	if (FileController)
	{
		FileController->Save();
	}

	// Close dropdown menu after action
	if (TopBarController)
	{
		TopBarController->CloseFilePanel();
	}
}

void UFileWidget::OnSaveAsClicked()
{
	if (FileController)
	{
		FileController->SaveAs(TEXT(""));
	}

	// Close dropdown menu after action
	if (TopBarController)
	{
		TopBarController->CloseFilePanel();
	}
}

void UFileWidget::OnMergeClicked()
{
	if (FileController)
	{
		FileController->Merge();
	}

	// Close dropdown menu after action
	if (TopBarController)
	{
		TopBarController->CloseFilePanel();
	}
}

void UFileWidget::OnQuitClicked()
{
	if (FileController)
	{
		FileController->Quit();
	}

	// Close dropdown menu after action (Quit will close app anyway)
	if (TopBarController)
	{
		//TopBarController->CloseFileMenu();
	}
}

// Model Event Handlers - Update View

void UFileWidget::OnDataChanged()
{
	RefreshUI();
}

void UFileWidget::OnRecentFilesChanged(const TArray<FString>& RecentFiles)
{
	PopulateRecentFilesList(RecentFiles);
}

void UFileWidget::OnCurrentWorldChanged(const FString& WorldPath)
{
	UpdateCurrentFileDisplay(WorldPath);
}

void UFileWidget::OnDirtyStateChanged(bool bIsDirty)
{
	UpdateUnsavedIndicator(bIsDirty);
}

void UFileWidget::OnFileError(const FString& ErrorMessage)
{
	UE_LOG(LogTemp, Error, TEXT("FileWidget: Error - %s"), *ErrorMessage);

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

			//아직 못함
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
