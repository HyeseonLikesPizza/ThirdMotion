// Fill out your copyright notice in the Description page of Project Settings.

#include "UI/Widget/UserListItem.h"
#include "UI/Widget/UserList.h"
#include "Components/TextBlock.h"

void UUserListItem::NativeConstruct()
{
	Super::NativeConstruct();
}

void UUserListItem::NativeOnListItemObjectSet(UObject* ListItem)
{
	UserObject = Cast<UConnectedUserObject>(ListItem);

	if (UserObject)
	{
		const FConnectedUserData& UserData = UserObject->GetUserData();

		// Set player name
		if (PlayerNameText)
		{
			PlayerNameText->SetText(FText::FromString(UserData.PlayerName));
		}

		// Set IP address
		if (IPAddressText)
		{
			IPAddressText->SetText(FText::FromString(UserData.IPAddress));
		}

		// Set player ID
		if (PlayerIDText)
		{
			FString IDText = FString::Printf(TEXT("ID: %d"), UserData.PlayerId);
			PlayerIDText->SetText(FText::FromString(IDText));
		}

		UE_LOG(LogTemp, Log, TEXT("UserListItem set: %s (IP: %s)"),
			*UserData.PlayerName, *UserData.IPAddress);
	}
}
