// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UI/WidgetController/BaseWidgetController.h"
#include "TopBarController.generated.h"

/**
 * Controller: TopBarController
 * Handles TopBar UI state and business logic
 */
UCLASS()
class THIRDMOTION_API UTopBarController : public UBaseWidgetController
{
	GENERATED_BODY()

public:
	UTopBarController();

	virtual void Init() override;

	// Toggle CanvasPanelFile visibility
	UFUNCTION()
	void ToggleFilePanel();

	// Close CanvasPanelFile
	UFUNCTION()
	void CloseFilePanel();

	// Open CanvasPanelFile
	UFUNCTION()
	void OpenFilePanel();

	// Check if file panel is open
	UFUNCTION()
	bool IsFilePanelOpen() const { return bIsFilePanelOpen; }

	// Events - Notify View of state changes
	DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnFilePanelStateChanged, bool, bIsOpen);

	UPROPERTY(BlueprintAssignable, Category = "TopBar Events")
	FOnFilePanelStateChanged OnFilePanelStateChanged;

protected:
	// File panel state
	bool bIsFilePanelOpen;
};
