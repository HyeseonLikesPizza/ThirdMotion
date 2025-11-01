// Fill out your copyright notice in the Description page of Project Settings.

#include "UI/Widget/UserList.h"
#include "Components/TextBlock.h"
#include "Components/Button.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/PlayerController.h"
#include "GameFramework/GameStateBase.h"
#include "GameFramework/PlayerState.h"
#include "Engine/World.h"
#include "Engine/NetConnection.h"
#include "OnlineSubsystem.h"
#include "Interfaces/OnlineSessionInterface.h"
#include "OnlineSessionSettings.h"

void UUserList::NativeConstruct()
{
	Super::NativeConstruct();

	// Initialize settings
	RefreshInterval = 1.0f; // Refresh every 1 second
	TimeSinceLastRefresh = 0.0f;
	bAutoRefresh = true;
	bIsVoiceChatActive = false; // 음성 채팅 초기 상태는 비활성

	// Voice Interface 초기화 (OnlineSubsystem)
	IOnlineSubsystem* OnlineSubsystem = IOnlineSubsystem::Get();
	if (OnlineSubsystem)
	{
		VoiceInterface = OnlineSubsystem->GetVoiceInterface();
		if (VoiceInterface.IsValid())
		{
			UE_LOG(LogTemp, Log, TEXT("UserList: Voice Interface initialized"));
		}
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("UserList: Voice Interface not available"));
		}
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("UserList: OnlineSubsystem not available"));
	}

	// Set title
	if (TitleText)
	{
		TitleText->SetText(FText::FromString(TEXT("Connected Users")));
	}

	// Bind button events
	if (StartButton)
	{
		StartButton->OnClicked.AddDynamic(this, &UUserList::OnStartButtonClicked);
		UE_LOG(LogTemp, Log, TEXT("UserList: StartButton bound"));
	}
	if (StopButton)
	{
		StopButton->OnClicked.AddDynamic(this, &UUserList::OnStopButtonClicked);
		UE_LOG(LogTemp, Log, TEXT("UserList: StopButton bound"));
	}

	// Initial refresh
	RefreshUserList();
}

void UUserList::OnStartButtonClicked()
{
	// PlayerController 가져오기
	APlayerController* PC = GetOwningPlayer();
	if (!PC)
	{
		UE_LOG(LogTemp, Warning, TEXT("UserList: PlayerController not found"));
		return;
	}

	// 네트워크 모드 확인
	UWorld* World = GetWorld();
	if (!World)
	{
		UE_LOG(LogTemp, Error, TEXT("UserList: World not available"));
		return;
	}

	ENetMode NetMode = World->GetNetMode();
	if (NetMode == NM_Standalone)
	{
		UE_LOG(LogTemp, Error, TEXT("UserList: Voice Chat requires multiplayer connection"));
		return;
	}

	// 음성 채팅 시작
	if (!bIsVoiceChatActive)
	{
		// VoiceInterface를 통해 네트워크 음성 시작
		if (VoiceInterface.IsValid())
		{
			const ULocalPlayer* LocalPlayer = PC->GetLocalPlayer();
			if (LocalPlayer && LocalPlayer->GetPreferredUniqueNetId().IsValid())
			{
				VoiceInterface->StartNetworkedVoice(0); // 로컬 유저 인덱스 0
				UE_LOG(LogTemp, Log, TEXT("UserList: Networked voice started via VoiceInterface"));
			}
		}

		// PlayerController 음성 활성화
		PC->StartTalking();
		bIsVoiceChatActive = true;
		UE_LOG(LogTemp, Log, TEXT("UserList: Voice chat started"));

		// StartButton 비활성화, StopButton 활성화
		if (StartButton)
		{
			StartButton->SetIsEnabled(false);
		}
		if (StopButton)
		{
			StopButton->SetIsEnabled(true);
		}
	}
}

void UUserList::OnStopButtonClicked()
{
	// PlayerController 가져오기
	APlayerController* PC = GetOwningPlayer();
	if (!PC)
	{
		UE_LOG(LogTemp, Warning, TEXT("UserList: PlayerController not found"));
		return;
	}

	// 음성 채팅 종료
	if (bIsVoiceChatActive)
	{
		// VoiceInterface를 통해 네트워크 음성 종료
		if (VoiceInterface.IsValid())
		{
			VoiceInterface->StopNetworkedVoice(0); // 로컬 유저 인덱스 0
			UE_LOG(LogTemp, Log, TEXT("UserList: Networked voice stopped via VoiceInterface"));
		}

		// PlayerController 음성 비활성화
		PC->StopTalking();
		bIsVoiceChatActive = false;
		UE_LOG(LogTemp, Log, TEXT("UserList: Voice chat stopped"));

		// StopButton 비활성화, StartButton 활성화
		if (StopButton)
		{
			StopButton->SetIsEnabled(false);
		}
		if (StartButton)
		{
			StartButton->SetIsEnabled(true);
		}
	}
}

void UUserList::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);

	if (bAutoRefresh)
	{
		TimeSinceLastRefresh += InDeltaTime;

		if (TimeSinceLastRefresh >= RefreshInterval)
		{
			RefreshUserList();
			TimeSinceLastRefresh = 0.0f;
		}
	}
}

void UUserList::RefreshUserList()
{
	// Get all connected users
	TArray<FConnectedUserData> Users = GetConnectedUsers();

	// Update the user list display
	UpdateUserList();
}

void UUserList::UpdateUserList()
{
	// Get all connected users
	TArray<FConnectedUserData> Users = GetConnectedUsers();

	// Build a formatted string with all user information
	FString UserListString = TEXT("");

	for (int32 i = 0; i < Users.Num(); i++)
	{
		const FConnectedUserData& User = Users[i];

		// Format: "Player Name (ID: X) - IP: xxx.xxx.xxx.xxx"
		FString UserLine = FString::Printf(TEXT("%s (ID: %d) - IP: %s\n"),
			*User.PlayerName,
			User.PlayerId,
			*User.IPAddress);

		UserListString += UserLine;
	}

	// Update the text block
	if (UserListText)
	{
		UserListText->SetText(FText::FromString(UserListString));
	}

	// Update user count
	if (UserCountText)
	{
		int32 UserCount = Users.Num();
		FString CountText = FString::Printf(TEXT("Total Users: %d"), UserCount);
		UserCountText->SetText(FText::FromString(CountText));
	}

	UE_LOG(LogTemp, Log, TEXT("Updated user list. Total users: %d"), Users.Num());
}

TArray<FConnectedUserData> UUserList::GetConnectedUsers()
{
	TArray<FConnectedUserData> Users;

	UWorld* World = GetWorld();
	if (!World)
	{
		UE_LOG(LogTemp, Warning, TEXT("World is null!"));
		return Users;
	}

	AGameStateBase* GameState = World->GetGameState();
	if (!GameState)
	{
		UE_LOG(LogTemp, Warning, TEXT("GameState is null!"));
		return Users;
	}

	// Iterate through all player states
	for (APlayerState* PlayerState : GameState->PlayerArray)
	{
		if (PlayerState)
		{
			FConnectedUserData UserData;

			// Get player name
			UserData.PlayerName = PlayerState->GetPlayerName();

			// Get player ID
			UserData.PlayerId = PlayerState->GetPlayerId();

			// Get IP address from player controller
			APlayerController* PC = Cast<APlayerController>(PlayerState->GetOwner());
			if (PC)
			{
				UserData.IPAddress = GetPlayerIPAddress(PC);
			}
			else
			{
				UserData.IPAddress = TEXT("Local/Unknown");
			}

			// Get connected time
			UserData.ConnectedTime = World->GetTimeSeconds() - PlayerState->GetStartTime();

			Users.Add(UserData);

			UE_LOG(LogTemp, Log, TEXT("Found player: %s (ID: %d, IP: %s)"),
				*UserData.PlayerName, UserData.PlayerId, *UserData.IPAddress);
		}
	}

	return Users;
}

FString UUserList::GetPlayerIPAddress(APlayerController* PlayerController)
{
	if (!PlayerController)
		return TEXT("Unknown");

	// If this is a local player controller
	if (PlayerController->IsLocalController())
	{
		return TEXT("127.0.0.1 (Local)");
	}

	// Get the network connection
	UNetConnection* NetConnection = Cast<UNetConnection>(PlayerController->Player);
	if (NetConnection)
	{
		// Get the remote address
		FString Address = NetConnection->LowLevelGetRemoteAddress(true);

		if (!Address.IsEmpty())
		{
			// Parse the IP address (format is usually "IP:Port")
			FString IPAddress;
			FString Port;
			if (Address.Split(TEXT(":"), &IPAddress, &Port))
			{
				return IPAddress;
			}
			return Address;
		}
	}

	// Fallback: try to get from URL
	if (PlayerController->Player)
	{
		// For remote players, try to extract from connection
		return TEXT("Remote Player");
	}

	return TEXT("Unknown");
}
