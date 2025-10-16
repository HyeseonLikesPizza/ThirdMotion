// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UI/Widget/BaseWidget.h"
#include "Components/Button.h"
#include "Components/EditableTextBox.h"
#include "TopBar.generated.h"

class USaveGameManager;

/**
 * Top Bar - Contains menu buttons including Save/Load functionality
 */
UCLASS()
class THIRDMOTION_API UTopBar : public UBaseWidget
{
	GENERATED_BODY()

public:
	virtual void NativeConstruct() override;
	virtual void NativeDestruct() override;

	// Widget Components
	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	UButton* SaveButton;

	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	UButton* LoadButton;

	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	UButton* SaveAsButton;

	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	UButton* QuickSaveButton;

	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	UButton* QuickLoadButton;

	// Optional: Text box for Save As functionality
	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	UEditableTextBox* SaveNameTextBox;

	// Events
	DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnSaveRequested, const FString&, SaveName);
	DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnLoadRequested, const FString&, SaveName);

	UPROPERTY(BlueprintAssignable, Category = "TopBar Events")
	FOnSaveRequested OnSaveRequested;

	UPROPERTY(BlueprintAssignable, Category = "TopBar Events")
	FOnLoadRequested OnLoadRequested;

protected:
	// SaveGameManager reference
	UPROPERTY()
	USaveGameManager* SaveGameManager;

	// Button click handlers
	UFUNCTION()
	void OnSaveClicked();

	UFUNCTION()
	void OnLoadClicked();

	UFUNCTION()
	void OnSaveAsClicked();

	UFUNCTION()
	void OnQuickSaveClicked();

	UFUNCTION()
	void OnQuickLoadClicked();

	// SaveGameManager event callbacks
	UFUNCTION()
	void OnSceneSaved(const FString& SlotName);

	UFUNCTION()
	void OnSceneLoaded(const FString& SlotName);

	UFUNCTION()
	void OnSaveError(const FString& ErrorMessage, const FString& SlotName);
};
