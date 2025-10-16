// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UI/Widget/BaseWidget.h"
#include "Components/Button.h"
#include "BottomBar.generated.h"

/**
 * Bottom Bar - Contains buttons for switching panels
 */
UCLASS()
class THIRDMOTION_API UBottomBar : public UBaseWidget
{
	GENERATED_BODY()

public:
	virtual void NativeConstruct() override;

	// Widget Components
	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	UButton* LibraryButton;

	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	UButton* SceneButton;

	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	UButton* PropertiesButton;

	// Events for panel switching
	DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnLibraryButtonClicked);
	DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnSceneButtonClicked);
	DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnPropertiesButtonClicked);

	UPROPERTY(BlueprintAssignable, Category = "BottomBar Events")
	FOnLibraryButtonClicked OnLibraryButtonClicked;

	UPROPERTY(BlueprintAssignable, Category = "BottomBar Events")
	FOnSceneButtonClicked OnSceneButtonClicked;

	UPROPERTY(BlueprintAssignable, Category = "BottomBar Events")
	FOnPropertiesButtonClicked OnPropertiesButtonClicked;

protected:
	// Button click handlers
	UFUNCTION()
	void OnLibraryClicked();

	UFUNCTION()
	void OnSceneClicked();

	UFUNCTION()
	void OnPropertiesClicked();
};
