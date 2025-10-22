// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UI/WidgetController/BaseWidgetController.h"
#include "FileController.generated.h"

class UFileDataModel;

/**
 * Controller: FileController
 * Handles all file-related business logic and actions
 */
UCLASS()
class THIRDMOTION_API UFileController : public UBaseWidgetController
{
	GENERATED_BODY()

public:
	UFileController();

	virtual void Init() override;

	// Set the data model
	UFUNCTION()
	void SetDataModel(UFileDataModel* InDataModel);

	// File Operations
	UFUNCTION()
	void NewScene();

	UFUNCTION()
	void Open();

	UFUNCTION()
	void OpenRecent(const FString& FilePath);

	UFUNCTION()
	void ShowHome();

	UFUNCTION()
	void Save();

	UFUNCTION()
	void SaveAs(const FString& NewFileName);

	UFUNCTION()
	void Merge();

	UFUNCTION()
	void Quit();

	// Helper Functions
	UFUNCTION()
	bool CheckUnsavedChanges();

	UFUNCTION()
	void ShowSaveDialog();

	// Get data model
	UFUNCTION()
	UFileDataModel* GetDataModel() const { return DataModel; }

	// Events - Notify View of actions
	DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnNewSceneRequested);
	DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnSaveCompleted);
	DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnSaveAsCompleted, const FString&, NewPath);
	DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnFileOpened, const FString&, FilePath);
	DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnError, const FString&, ErrorMessage);
	DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnQuitRequested, bool, bHasUnsavedChanges);

	UPROPERTY(BlueprintAssignable, Category = "File Events")
	FOnNewSceneRequested OnNewSceneRequested;

	UPROPERTY(BlueprintAssignable, Category = "File Events")
	FOnSaveCompleted OnSaveCompleted;

	UPROPERTY(BlueprintAssignable, Category = "File Events")
	FOnSaveAsCompleted OnSaveAsCompleted;

	UPROPERTY(BlueprintAssignable, Category = "File Events")
	FOnFileOpened OnFileOpened;

	UPROPERTY(BlueprintAssignable, Category = "File Events")
	FOnError OnError;

	UPROPERTY(BlueprintAssignable, Category = "File Events")
	FOnQuitRequested OnQuitRequested;

protected:
	// Data Model reference
	UPROPERTY()
	UFileDataModel* DataModel;

	// Project File folder path
	FString GetProjectFileFolderPath() const;

	// Epic Games Launcher path
	FString GetEpicGamesLauncherPath() const;
};
