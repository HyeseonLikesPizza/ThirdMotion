// TwinmotionSceneTreeItem.h - Scene hierarchy tree item widget
// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "UI/BaseWidget.h"
#include "Components/Button.h"
#include "Components/Image.h"
#include "Components/EditableText.h"
#include "Components/HorizontalBox.h"
#include "GameFramework/Actor.h"
#include "SceneTreeItem.generated.h"

/**
 * Data structure for scene tree items
 */
USTRUCT(BlueprintType)
struct FSceneTreeItemData
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Scene Tree")
	FString Name;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Scene Tree")
	AActor* Actor;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Scene Tree")
	int32 IndentLevel;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Scene Tree")
	bool bIsExpanded;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Scene Tree")
	bool bIsVisible;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Scene Tree")
	bool bHasChildren;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Scene Tree")
	UTexture2D* Icon;

	FSceneTreeItemData()
		: Name(TEXT("Untitled"))
		, Actor(nullptr)
		, IndentLevel(0)
		, bIsExpanded(false)
		, bIsVisible(true)
		, bHasChildren(false)
		, Icon(nullptr)
	{
	}
};

/**
 * Widget for displaying a single scene tree item
 */
UCLASS()
class THIRDMOTION_API USceneTreeItem : public UBaseWidget
{
	GENERATED_BODY()

public:
	virtual void NativeConstruct() override;

	// Widget Components
	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	UHorizontalBox* ItemContainer;

	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	UButton* ExpandButton;

	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	UImage* IconImage;

	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	UEditableText* NameText;

	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	UButton* VisibilityButton;

	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	UButton* SelectButton;

	// Set item data
	UFUNCTION(BlueprintCallable, Category = "Scene Tree")
	void SetSceneTreeItemData(const FSceneTreeItemData& InData);

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Scene Tree")
	FSceneTreeItemData GetSceneTreeItemData() const { return ItemData; }

	// Expand/Collapse
	UFUNCTION(BlueprintCallable, Category = "Scene Tree")
	void SetExpanded(bool bExpanded);

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Scene Tree")
	bool IsExpanded() const { return ItemData.bIsExpanded; }

	// Visibility
	UFUNCTION(BlueprintCallable, Category = "Scene Tree")
	void SetItemVisible(bool bVisible);

	// Selection
	UFUNCTION(BlueprintCallable, Category = "Scene Tree")
	void SetSelected(bool bSelected);

	// Events
	DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnTreeItemSelected, const FSceneTreeItemData&, ItemData);
	DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnTreeItemExpanded, const FSceneTreeItemData&, ItemData, bool, bExpanded);
	DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnTreeItemVisibilityChanged, const FSceneTreeItemData&, ItemData, bool, bVisible);
	DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnTreeItemRenamed, const FSceneTreeItemData&, ItemData, const FString&, NewName);

	UPROPERTY(BlueprintAssignable, Category = "Scene Tree Events")
	FOnTreeItemSelected OnTreeItemSelected;

	UPROPERTY(BlueprintAssignable, Category = "Scene Tree Events")
	FOnTreeItemExpanded OnTreeItemExpanded;

	UPROPERTY(BlueprintAssignable, Category = "Scene Tree Events")
	FOnTreeItemVisibilityChanged OnTreeItemVisibilityChanged;

	UPROPERTY(BlueprintAssignable, Category = "Scene Tree Events")
	FOnTreeItemRenamed OnTreeItemRenamed;

protected:
	UPROPERTY(BlueprintReadOnly, Category = "Scene Tree")
	FSceneTreeItemData ItemData;

	UPROPERTY()
	bool bIsSelected;

	// Button callbacks
	UFUNCTION()
	void OnExpandButtonClicked();

	UFUNCTION()
	void OnVisibilityButtonClicked();

	UFUNCTION()
	void OnSelectButtonClicked();

	UFUNCTION()
	void OnNameTextCommitted(const FText& Text, ETextCommit::Type CommitMethod);

	// Visual updates
	UFUNCTION(BlueprintImplementableEvent, Category = "Scene Tree")
	void OnItemSelectedChanged(bool bSelected);

	UFUNCTION(BlueprintImplementableEvent, Category = "Scene Tree")
	void OnItemExpandedChanged(bool bExpanded);

	UFUNCTION(BlueprintImplementableEvent, Category = "Scene Tree")
	void OnItemVisibilityChanged(bool bVisible);

	void UpdateIndentation();
	void UpdateExpandButton();
	void UpdateVisibilityButton();
};
