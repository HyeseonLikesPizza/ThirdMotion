// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UI/Widget/BaseWidget.h"
#include "Components/Button.h"
#include "Components/TextBlock.h"
#include "Components/VerticalBox.h"
#include "FileWidget.generated.h"

class UFileController;
class UFileDataModel;

/**
 * View: FileWidget
 * Displays file menu UI and delegates user input to FileController
 */
UCLASS()
class THIRDMOTION_API UFileWidget : public UBaseWidget
{
	GENERATED_BODY()

public:
	virtual void NativeConstruct() override;
	virtual void NativeDestruct() override;

	// Setup with controller and model
	UFUNCTION()
	void SetupMVC(UFileController* InController, UFileDataModel* InDataModel);

	// Refresh UI based on model data
	UFUNCTION()
	void RefreshUI();

	// Widget Components - File Menu Buttons
	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	UButton* NewSceneButton;

	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	UButton* OpenButton;

	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	UButton* OpenRecentButton;

	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	UButton* ShowHomeButton;

	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	UButton* SaveButton;

	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	UButton* SaveAsButton;

	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	UButton* MergeButton;

	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	UButton* QuitButton;

	// Recent files list container
	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	UVerticalBox* RecentFilesContainer;

	// Current file path display
	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	UTextBlock* CurrentFileText;

	// Unsaved changes indicator
	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	UTextBlock* UnsavedIndicator;

protected:
	// Controller reference (handles business logic)
	UPROPERTY()
	UFileController* FileController;

	// Data Model reference
	UPROPERTY()
	UFileDataModel* DataModel;

	// Button click handlers - Delegate to Controller
	UFUNCTION()
	void OnNewSceneClicked();

	UFUNCTION()
	void OnOpenClicked();

	UFUNCTION()
	void OnOpenRecentClicked();

	UFUNCTION()
	void OnShowHomeClicked();

	UFUNCTION()
	void OnSaveClicked();

	UFUNCTION()
	void OnSaveAsClicked();

	UFUNCTION()
	void OnMergeClicked();

	UFUNCTION()
	void OnQuitClicked();

	// Model event handlers - Update View
	UFUNCTION()
	void OnDataChanged();

	UFUNCTION()
	void OnRecentFilesChanged(const TArray<FString>& RecentFiles);

	UFUNCTION()
	void OnCurrentWorldChanged(const FString& WorldPath);

	UFUNCTION()
	void OnDirtyStateChanged(bool bIsDirty);

	// Controller event handlers
	UFUNCTION()
	void OnFileError(const FString& ErrorMessage);

	// Helper functions
	void PopulateRecentFilesList(const TArray<FString>& RecentFiles);
	void UpdateCurrentFileDisplay(const FString& FilePath);
	void UpdateUnsavedIndicator(bool bIsDirty);

	// Recent file button class for dynamic creation
	UPROPERTY(EditDefaultsOnly, Category = "File Widget")
	TSubclassOf<UButton> RecentFileButtonClass;
};
