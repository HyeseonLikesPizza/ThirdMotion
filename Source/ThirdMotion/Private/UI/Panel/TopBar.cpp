// Fill out your copyright notice in the Description page of Project Settings.

#include "UI/Panel/TopBar.h"
#include "UI/Widget/FileWidget.h"
#include "UI/WidgetController/FileController.h"
#include "UI/WidgetController/TopBarController.h"
#include "Data/FileDataModel.h"
#include "Components/Button.h"
#include "Components/CanvasPanel.h"

void UTopBar::NativeConstruct()
{
	Super::NativeConstruct();

	// Setup TopBar MVC system
	SetupTopBarMVC();

	// Setup File MVC system
	SetupFileMVC();

	// Bind File button
	if (FileButton)
	{
		FileButton->OnClicked.AddDynamic(this, &UTopBar::OnFileButtonClicked);
	}

	// Initially hide CanvasPanelFile
	if (CanvasPanelFile)
	{
		CanvasPanelFile->SetVisibility(ESlateVisibility::Hidden);
		UE_LOG(LogTemp, Log, TEXT("TopBar: CanvasPanelFile initially hidden"));
	}

	UE_LOG(LogTemp, Log, TEXT("TopBar constructed"));
}

void UTopBar::NativeDestruct()
{
	// Unbind from TopBarController events
	if (TopBarController)
	{
		TopBarController->OnFilePanelStateChanged.RemoveDynamic(this, &UTopBar::OnFilePanelStateChanged);
	}

	Super::NativeDestruct();
}

void UTopBar::SetupTopBarMVC()
{
	// Create TopBar Controller
	TopBarController = NewObject<UTopBarController>(this);
	if (TopBarController)
	{
		TopBarController->Init();

		// Bind to controller events (Observer Pattern)
		TopBarController->OnFilePanelStateChanged.AddDynamic(this, &UTopBar::OnFilePanelStateChanged);

		UE_LOG(LogTemp, Log, TEXT("TopBar: TopBarController created"));
	}
}

void UTopBar::SetupFileMVC()
{
	// Create Model
	FileDataModel = NewObject<UFileDataModel>(this);
	if (FileDataModel)
	{
		FileDataModel->LoadRecentFilesFromConfig();
		UE_LOG(LogTemp, Log, TEXT("TopBar: FileDataModel created"));
	}

	// Create Controller
	FileController = NewObject<UFileController>(this);
	if (FileController)
	{
		FileController->Init();
		FileController->SetDataModel(FileDataModel);
		UE_LOG(LogTemp, Log, TEXT("TopBar: FileController created"));
	}

	// Setup FileWidget View with MVC
	if (FileWidget)
	{
		FileWidget->SetupMVC(FileController, FileDataModel);
		FileWidget->SetTopBarController(TopBarController);
		UE_LOG(LogTemp, Log, TEXT("TopBar: FileWidget MVC setup completed"));
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("TopBar: FileWidget is null - make sure to bind it in Blueprint"));
	}
}

void UTopBar::OnFileButtonClicked()
{
	UE_LOG(LogTemp, Log, TEXT("TopBar: File button clicked"));

	// Delegate to Controller
	if (TopBarController)
	{
		TopBarController->ToggleFilePanel();
	}
}

void UTopBar::OnFilePanelStateChanged(bool bIsOpen)
{
	// Update View based on Controller state (Observer Pattern)
	if (!CanvasPanelFile)
	{
		UE_LOG(LogTemp, Warning, TEXT("TopBar: CanvasPanelFile is null"));
		return;
	}

	if (bIsOpen)
	{
		// Show CanvasPanelFile
		CanvasPanelFile->SetVisibility(ESlateVisibility::Visible);
		UE_LOG(LogTemp, Log, TEXT("TopBar View: CanvasPanelFile set to Visible"));
	}
	else
	{
		// Hide CanvasPanelFile
		CanvasPanelFile->SetVisibility(ESlateVisibility::Hidden);
		UE_LOG(LogTemp, Log, TEXT("TopBar View: CanvasPanelFile set to Hidden"));
	}
}

// DEPRECATED: Use TopBarController instead
void UTopBar::ToggleFileMenu()
{
	if (TopBarController)
	{
		TopBarController->ToggleFilePanel();
	}
}

// DEPRECATED: Use TopBarController instead
void UTopBar::CloseFileMenu()
{
	if (TopBarController)
	{
		TopBarController->CloseFilePanel();
	}
}
