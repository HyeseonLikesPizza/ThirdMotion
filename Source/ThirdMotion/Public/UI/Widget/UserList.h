// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UI/Widget/BaseWidget.h"
#include "Components/TextBlock.h"
#include "UserList.generated.h"

/**
 * Data structure for connected user information
 */
USTRUCT(BlueprintType)
struct FConnectedUserData
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Network")
	FString IPAddress;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Network")
	FString PlayerName;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Network")
	int32 PlayerId;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Network")
	float ConnectedTime;

	FConnectedUserData()
		: IPAddress(TEXT("Unknown"))
		, PlayerName(TEXT("Player"))
		, PlayerId(0)
		, ConnectedTime(0.0f)
	{
	}

	FConnectedUserData(const FString& InIP, const FString& InName, int32 InId)
		: IPAddress(InIP)
		, PlayerName(InName)
		, PlayerId(InId)
		, ConnectedTime(0.0f)
	{
	}
};

/**
 * Object wrapper for connected user data (for use with ListView)
 */
UCLASS(BlueprintType)
class THIRDMOTION_API UConnectedUserObject : public UObject
{
	GENERATED_BODY()

public:
	UPROPERTY(BlueprintReadWrite, Category = "Network")
	FConnectedUserData UserData;

	void SetUserData(const FConnectedUserData& InData)
	{
		UserData = InData;
	}

	FConnectedUserData GetUserData() const
	{
		return UserData;
	}
};

/**
 * Widget for displaying list of connected users and their IP addresses
 */
UCLASS()
class THIRDMOTION_API UUserList : public UBaseWidget
{
	GENERATED_BODY()

public:
	virtual void NativeConstruct() override;
	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;

	// Widget Components
	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	UTextBlock* UserListText;

	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	UTextBlock* TitleText;

	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	UTextBlock* UserCountText;

	// Functions to manage user list
	UFUNCTION(BlueprintCallable, Category = "Network")
	void RefreshUserList();

	UFUNCTION(BlueprintCallable, Category = "Network")
	void UpdateUserList();

	// Get connected users from game state/player controller
	UFUNCTION(BlueprintCallable, Category = "Network")
	TArray<FConnectedUserData> GetConnectedUsers();

protected:

	// Auto-refresh timer
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Network")
	float RefreshInterval;

	UPROPERTY()
	float TimeSinceLastRefresh;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Network")
	bool bAutoRefresh;

	// Helper function to extract IP from player controller
	FString GetPlayerIPAddress(class APlayerController* PlayerController);
};
