// Fill out your copyright notice in the Description page of Project Settings.

#include "UI/Panel/TopBar.h"
#include "Save/SaveGameManager.h"
#include "Components/Button.h"
#include "Components/EditableTextBox.h"

void UTopBar::NativeConstruct()
{
	Super::NativeConstruct();

	// Get SaveGameManager subsystem
	if (GetGameInstance())
	{
		SaveGameManager = GetGameInstance()->GetSubsystem<USaveGameManager>();
		if (SaveGameManager)
		{
			// Bind to SaveGameManager events (Observer Pattern)
			SaveGameManager->OnSceneSaved.AddDynamic(this, &UTopBar::OnSceneSaved);
			SaveGameManager->OnSceneLoaded.AddDynamic(this, &UTopBar::OnSceneLoaded);
			SaveGameManager->OnSaveError.AddDynamic(this, &UTopBar::OnSaveError);

			UE_LOG(LogTemp, Log, TEXT("TopBar: Connected to SaveGameManager"));
		}
		else
		{
			UE_LOG(LogTemp, Error, TEXT("TopBar: Failed to get SaveGameManager!"));
		}
	}

	// Bind button events
	if (SaveButton)
	{
		SaveButton->OnClicked.AddDynamic(this, &UTopBar::OnSaveClicked);
	}

	if (LoadButton)
	{
		LoadButton->OnClicked.AddDynamic(this, &UTopBar::OnLoadClicked);
	}

	if (SaveAsButton)
	{
		SaveAsButton->OnClicked.AddDynamic(this, &UTopBar::OnSaveAsClicked);
	}

	if (QuickSaveButton)
	{
		QuickSaveButton->OnClicked.AddDynamic(this, &UTopBar::OnQuickSaveClicked);
	}

	if (QuickLoadButton)
	{
		QuickLoadButton->OnClicked.AddDynamic(this, &UTopBar::OnQuickLoadClicked);
	}

	UE_LOG(LogTemp, Log, TEXT("TopBar constructed with Save/Load buttons"));
}

void UTopBar::NativeDestruct()
{
	// Unbind from SaveGameManager events
	if (SaveGameManager)
	{
		SaveGameManager->OnSceneSaved.RemoveDynamic(this, &UTopBar::OnSceneSaved);
		SaveGameManager->OnSceneLoaded.RemoveDynamic(this, &UTopBar::OnSceneLoaded);
		SaveGameManager->OnSaveError.RemoveDynamic(this, &UTopBar::OnSaveError);
	}

	Super::NativeDestruct();
}

void UTopBar::OnSaveClicked()
{
	if (!SaveGameManager)
	{
		UE_LOG(LogTemp, Error, TEXT("TopBar: SaveGameManager is null!"));
		return;
	}

	UE_LOG(LogTemp, Log, TEXT("TopBar: Save button clicked"));

	// Save to default AutoSave slot
	SaveGameManager->SaveScene(TEXT("MyScene"));

	// Broadcast event
	OnSaveRequested.Broadcast(TEXT("AutoSave"));
}

void UTopBar::OnLoadClicked()
{
	if (!SaveGameManager)
	{
		UE_LOG(LogTemp, Error, TEXT("TopBar: SaveGameManager is null!"));
		return;
	}

	UE_LOG(LogTemp, Log, TEXT("TopBar: Load button clicked"));

	// Load from default AutoSave slot
	SaveGameManager->LoadScene(TEXT("AutoSave"));

	// Broadcast event
	OnLoadRequested.Broadcast(TEXT("AutoSave"));
}

void UTopBar::OnSaveAsClicked()
{
	if (!SaveGameManager)
	{
		UE_LOG(LogTemp, Error, TEXT("TopBar: SaveGameManager is null!"));
		return;
	}

	UE_LOG(LogTemp, Log, TEXT("TopBar: Save As button clicked"));

	// Get custom name from text box
	FString CustomName = TEXT("MyScene");
	if (SaveNameTextBox)
	{
		FText NameText = SaveNameTextBox->GetText();
		if (!NameText.IsEmpty())
		{
			CustomName = NameText.ToString();
		}
	}

	// Save with custom slot name
	SaveGameManager->SaveSceneAs(CustomName, CustomName);

	// Broadcast event
	OnSaveRequested.Broadcast(CustomName);
}

void UTopBar::OnQuickSaveClicked()
{
	if (!SaveGameManager)
	{
		UE_LOG(LogTemp, Error, TEXT("TopBar: SaveGameManager is null!"));
		return;
	}

	UE_LOG(LogTemp, Log, TEXT("TopBar: Quick Save button clicked"));

	SaveGameManager->QuickSave();

	OnSaveRequested.Broadcast(TEXT("QuickSave"));
}

void UTopBar::OnQuickLoadClicked()
{
	if (!SaveGameManager)
	{
		UE_LOG(LogTemp, Error, TEXT("TopBar: SaveGameManager is null!"));
		return;
	}

	UE_LOG(LogTemp, Log, TEXT("TopBar: Quick Load button clicked"));

	SaveGameManager->QuickLoad();

	OnLoadRequested.Broadcast(TEXT("QuickLoad"));
}

void UTopBar::OnSceneSaved(const FString& SlotName)
{
	UE_LOG(LogTemp, Log, TEXT("TopBar: Scene saved to slot '%s'"), *SlotName);

	// TODO: Show success message to user
}

void UTopBar::OnSceneLoaded(const FString& SlotName)
{
	UE_LOG(LogTemp, Log, TEXT("TopBar: Scene loaded from slot '%s'"), *SlotName);

	// TODO: Show success message to user
}

void UTopBar::OnSaveError(const FString& ErrorMessage, const FString& SlotName)
{
	UE_LOG(LogTemp, Error, TEXT("TopBar: Save error - %s (Slot: %s)"), *ErrorMessage, *SlotName);

	// TODO: Show error popup to user
}
