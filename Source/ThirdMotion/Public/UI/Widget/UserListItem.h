// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UI/Widget/BaseWidget.h"
#include "Blueprint/IUserObjectListEntry.h"
#include "Components/TextBlock.h"
#include "UserListItem.generated.h"

class UConnectedUserObject;

/**
 * Widget for displaying a single user item in the user list
 */
UCLASS()
class THIRDMOTION_API UUserListItem : public UBaseWidget, public IUserObjectListEntry
{
	GENERATED_BODY()

public:
	virtual void NativeConstruct() override;
	virtual void NativeOnListItemObjectSet(UObject* ListItem) override;

	// Widget Components
	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	UTextBlock* PlayerNameText;

	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	UTextBlock* IPAddressText;

	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	UTextBlock* PlayerIDText;

protected:
	UPROPERTY()
	UConnectedUserObject* UserObject;
};
