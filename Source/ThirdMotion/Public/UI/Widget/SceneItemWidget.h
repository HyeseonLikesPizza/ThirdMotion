// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Blueprint/IUserObjectListEntry.h"
#include "SceneItemWidget.generated.h"

class USceneItemData;
class UTextBlock;
class UButton;
class UImage;

/**
 * 
 */
UCLASS()
class THIRDMOTION_API USceneItemWidget : public UUserWidget, public IUserObjectListEntry
{
	GENERATED_BODY()

protected:
	virtual void NativeConstruct() override;
	virtual void NativeDestruct() override;
	virtual void NativeOnListItemObjectSet(UObject* ListItemObject) override;

	UPROPERTY(meta = (BindWidget))
	UTextBlock* ItemLabel;

	UPROPERTY(meta = (BindWidget))
	UTextBlock* ItemType;

	UPROPERTY(meta = (BindWidget))
	UButton* VisibilityButton;

	UPROPERTY()
	USceneItemData* ItemData;

private:
	UFUNCTION()
	void OnModelDataChanged(USceneItemData* ChangedData);

	void UpdateView();

	UFUNCTION()
	void OnVisibilityButtonClicked();
};
