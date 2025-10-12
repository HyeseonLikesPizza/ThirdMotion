// TwinmotionBaseWidget.cpp
// Copyright Epic Games, Inc. All Rights Reserved.

#include "UI/TwinmotionBaseWidget.h"
#include "TwinmotionUIManager.h"
#include "Kismet/GameplayStatics.h"

void UTwinmotionBaseWidget::NativeConstruct()
{
	Super::NativeConstruct();

	// Get UI Manager reference
	UIManager = GetUIManager();

	if (UIManager)
	{
		OnTwinmotionUIInitialize();
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("TwinmotionBaseWidget: Failed to get UIManager reference"));
	}
}

void UTwinmotionBaseWidget::NativeDestruct()
{
	OnTwinmotionUICleanup();
	Super::NativeDestruct();
}

UTwinmotionUIManager* UTwinmotionBaseWidget::GetUIManager() const
{
	UGameInstance* GameInstance = GetGameInstance();
	if (GameInstance)
	{
		return GameInstance->GetSubsystem<UTwinmotionUIManager>();
	}
	return nullptr;
}
