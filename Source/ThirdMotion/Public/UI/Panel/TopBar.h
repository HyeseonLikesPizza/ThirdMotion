// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UI/Widget/BaseWidget.h"
#include "Components/Button.h"
#include "Components/EditableTextBox.h"
#include "TopBar.generated.h"

class USaveGameManager;
class UFileWidget;
class UFileController;
class UFileDataModel;
class UTopBarController;
class UCanvasPanel;

/**
 * Top Bar - Contains file menu and other top-level controls
 */
UCLASS()
class THIRDMOTION_API UTopBar : public UBaseWidget
{
	GENERATED_BODY()

public:
	virtual void NativeConstruct() override;
	virtual void NativeDestruct() override;

	// File button - Toggle file menu dropdown
	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	UButton* FileButton;

	// CanvasPanelFile - File menu dropdown container
	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	UCanvasPanel* CanvasPanelFile;

	// FileWidget - File menu panel (inside CanvasPanelFile)
	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	UFileWidget* FileWidget;

	// Toggle file menu visibility (DEPRECATED - use controller)
	UFUNCTION(BlueprintCallable, Category = "TopBar")
	void ToggleFileMenu();

	// Close file menu (DEPRECATED - use controller)
	UFUNCTION(BlueprintCallable, Category = "TopBar")
	void CloseFileMenu();

	// Get TopBar controller
	UFUNCTION(BlueprintCallable, Category = "TopBar")
	UTopBarController* GetTopBarController() const { return TopBarController; }

protected:
	// TopBar Controller (manages UI state)
	UPROPERTY()
	UTopBarController* TopBarController;

	// File MVC components
	UPROPERTY()
	UFileController* FileController;

	UPROPERTY()
	UFileDataModel* FileDataModel;

	// Setup TopBar MVC system
	void SetupTopBarMVC();

	// Setup File MVC system
	void SetupFileMVC();

	// Button click handlers
	UFUNCTION()
	void OnFileButtonClicked();

	// Controller event handlers (Observer Pattern)
	UFUNCTION()
	void OnFilePanelStateChanged(bool bIsOpen);
};
