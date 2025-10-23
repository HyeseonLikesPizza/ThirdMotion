// Fill out your copyright notice in the Description page of Project Settings.

#include "UI/WidgetController/TopBarController.h"

UTopBarController::UTopBarController()
{
	bIsFilePanelOpen = false;
}

void UTopBarController::Init()
{
	Super::Init();

	// Initialize state
	bIsFilePanelOpen = false;

	UE_LOG(LogTemp, Log, TEXT("TopBarController initialized"));
}

void UTopBarController::ToggleFilePanel()
{
	bIsFilePanelOpen = !bIsFilePanelOpen;

	// Broadcast state change to View
	OnFilePanelStateChanged.Broadcast(bIsFilePanelOpen);

	UE_LOG(LogTemp, Log, TEXT("TopBarController: File panel toggled to %s"),
		bIsFilePanelOpen ? TEXT("OPEN") : TEXT("CLOSED"));
}

void UTopBarController::CloseFilePanel()
{
	if (bIsFilePanelOpen)
	{
		bIsFilePanelOpen = false;
		OnFilePanelStateChanged.Broadcast(false);
		UE_LOG(LogTemp, Log, TEXT("TopBarController: File panel closed"));
	}
}

void UTopBarController::OpenFilePanel()
{
	if (!bIsFilePanelOpen)
	{
		bIsFilePanelOpen = true;
		OnFilePanelStateChanged.Broadcast(true);
		UE_LOG(LogTemp, Log, TEXT("TopBarController: File panel opened"));
	}
}
