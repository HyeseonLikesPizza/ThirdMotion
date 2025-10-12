// TwinmotionBaseWidget.h - Base class for all Twinmotion UI widgets
// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "TwinmotionBaseWidget.generated.h"

class UTwinmotionUIManager;

/**
 * Base class for all Twinmotion UI Widgets
 * Provides common functionality and access to UI Manager
 */
UCLASS(Abstract, Blueprintable)
class THIRDMOTION_API UTwinmotionBaseWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	virtual void NativeConstruct() override;
	virtual void NativeDestruct() override;

	// Get reference to UI Manager
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Twinmotion UI")
	UTwinmotionUIManager* GetUIManager() const;

protected:
	// Cached reference to UI Manager
	UPROPERTY(BlueprintReadOnly, Category = "Twinmotion UI")
	UTwinmotionUIManager* UIManager;

	// Override this in child classes for custom initialization
	UFUNCTION(BlueprintImplementableEvent, Category = "Twinmotion UI")
	void OnTwinmotionUIInitialize();

	// Override this in child classes for custom cleanup
	UFUNCTION(BlueprintImplementableEvent, Category = "Twinmotion UI")
	void OnTwinmotionUICleanup();
};
