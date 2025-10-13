// TwinmotionMainWidget.h - Main container widget for Twinmotion UI
// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "UI/BaseWidget.h"
#include "Components/Border.h"
#include "Components/CanvasPanel.h"
#include "Components/SizeBox.h"
#include "MainWidget.generated.h"

class UUIManager;
enum class ETwinmotionPanelType : uint8;

UCLASS()
class THIRDMOTION_API UMainWidget : public UBaseWidget
{
	GENERATED_BODY()

public:
	virtual void NativeConstruct() override;

	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	UUserWidget* TopBar;

	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	UUserWidget* LeftPanel;

	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	UUserWidget* RightPanel;

	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	UUserWidget* BottomPanel;

	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	UBorder* ViewportContainer;

	// Panel visibility controls
	UFUNCTION(BlueprintCallable, Category = "Twinmotion UI")
	void ToggleLeftPanel();

	UFUNCTION(BlueprintCallable, Category = "Twinmotion UI")
	void ToggleRightPanel();

	UFUNCTION(BlueprintCallable, Category = "Twinmotion UI")
	void ToggleBottomPanel();

	

protected:
	UFUNCTION()
	void OnPanelVisibilityChanged(ETwinmotionPanelType PanelType, bool bVisible);
};
