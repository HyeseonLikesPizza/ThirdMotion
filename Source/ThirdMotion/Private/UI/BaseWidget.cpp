// TwinmotionBaseWidget.cpp
// Copyright Epic Games, Inc. All Rights Reserved.

#include "UI/BaseWidget.h"
#include "UI/UIManager.h"
#include "Kismet/GameplayStatics.h"

void UBaseWidget::NativeConstruct()
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

void UBaseWidget::NativeDestruct()
{
	OnTwinmotionUICleanup();
	Super::NativeDestruct();
}

UUIManager* UBaseWidget::GetUIManager() const
{
	UGameInstance* GameInstance = GetGameInstance();
	if (GameInstance)
	{
		return GameInstance->GetSubsystem<UUIManager>();
	}
	return nullptr;
}
