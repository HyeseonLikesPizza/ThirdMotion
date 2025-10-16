// Fill out your copyright notice in the Description page of Project Settings.

#include "UI/Panel/BottomBar.h"
#include "Components/Button.h"

void UBottomBar::NativeConstruct()
{
	Super::NativeConstruct();

	// Bind button click events
	if (LibraryButton)
	{
		LibraryButton->OnClicked.AddDynamic(this, &UBottomBar::OnLibraryClicked);
	}

	if (SceneButton)
	{
		SceneButton->OnClicked.AddDynamic(this, &UBottomBar::OnSceneClicked);
	}

	if (PropertiesButton)
	{
		PropertiesButton->OnClicked.AddDynamic(this, &UBottomBar::OnPropertiesClicked);
	}

	UE_LOG(LogTemp, Log, TEXT("BottomBar constructed and buttons bound"));
}

void UBottomBar::OnLibraryClicked()
{
	OnLibraryButtonClicked.Broadcast();
	UE_LOG(LogTemp, Log, TEXT("BottomBar: Library button clicked"));
}

void UBottomBar::OnSceneClicked()
{
	OnSceneButtonClicked.Broadcast();
	UE_LOG(LogTemp, Log, TEXT("BottomBar: Scene button clicked"));
}

void UBottomBar::OnPropertiesClicked()
{
	OnPropertiesButtonClicked.Broadcast();
	UE_LOG(LogTemp, Log, TEXT("BottomBar: Properties button clicked"));
}
