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
#include "VoiceChat.h"

void UUserList::NativeConstruct()
{
	Super::NativeConstruct();

	// Initialize settings
	RefreshInterval = 1.0f; // Refresh every 1 second
	TimeSinceLastRefresh = 0.0f;
	bAutoRefresh = true;
	bIsVoiceChatActive = false; // 음성 채팅 초기 상태는 비활성

	// Voice Chat 인터페이스 초기화
	VoiceChatInterface = nullptr;
	VoiceChatUser = nullptr;
	ChannelName = TEXT("ThirdMotionChannel");
	PlayerDisplayName = TEXT("Player");

	// Voice Chat 초기화
	InitializeVoiceChat();

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
	UE_LOG(LogTemp, Log, TEXT("UserList: StartButton clicked"));

	if (!bIsVoiceChatActive)
	{
		ConnectToVoiceChannel();
	}
}

void UUserList::OnStopButtonClicked()
{
	UE_LOG(LogTemp, Log, TEXT("UserList: StopButton clicked"));

	if (bIsVoiceChatActive)
	{
		DisconnectFromVoiceChannel();
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

void UUserList::InitializeVoiceChat()
{
	// Voice Chat 인터페이스 가져오기
	VoiceChatInterface = IVoiceChat::Get();

	if (VoiceChatInterface)
	{
		UE_LOG(LogTemp, Log, TEXT("UserList: VoiceChat Interface obtained"));

		// VoiceChat 초기화
		if (!VoiceChatInterface->IsInitialized())
		{
			VoiceChatInterface->Initialize();
			UE_LOG(LogTemp, Log, TEXT("UserList: VoiceChat initialized"));
		}

		// VoiceChatUser 연결
		VoiceChatUser = VoiceChatInterface->CreateUser();
		if (VoiceChatUser)
		{
			UE_LOG(LogTemp, Log, TEXT("UserList: VoiceChatUser created"));

			// 플레이어 이름 설정
			APlayerController* PC = GetOwningPlayer();
			if (PC && PC->PlayerState)
			{
				PlayerDisplayName = PC->PlayerState->GetPlayerName();
			}

			// 로그인
			FPlatformUserId PlatformUserId = PLATFORMUSERID_NONE;
			if (PC && PC->GetLocalPlayer())
			{
				PlatformUserId = PC->GetLocalPlayer()->GetPlatformUserId();
			}

			VoiceChatUser->Login(PlatformUserId, PlayerDisplayName, TEXT(""),
				FOnVoiceChatLoginCompleteDelegate::CreateLambda([this](const FString& LoggedInPlayerName, const FVoiceChatResult& Result)
			{
				if (Result.IsSuccess())
				{
					UE_LOG(LogTemp, Log, TEXT("UserList: VoiceChat Login successful for %s"), *LoggedInPlayerName);
				}
				else
				{
					UE_LOG(LogTemp, Error, TEXT("UserList: VoiceChat Login failed: %s"), *Result.ErrorDesc);
				}
			}));
		}
		else
		{
			UE_LOG(LogTemp, Error, TEXT("UserList: Failed to create VoiceChatUser"));
		}
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("UserList: VoiceChat Interface not available"));
	}
}

void UUserList::ConnectToVoiceChannel()
{
	if (!VoiceChatUser)
	{
		UE_LOG(LogTemp, Error, TEXT("UserList: VoiceChatUser is null, cannot connect to channel"));
		return;
	}

	UE_LOG(LogTemp, Log, TEXT("UserList: Connecting to voice channel: %s"), *ChannelName);

	// 채널에 연결
	VoiceChatUser->JoinChannel(ChannelName, TEXT(""), EVoiceChatChannelType::NonPositional,
		FOnVoiceChatChannelJoinCompleteDelegate::CreateLambda([this](const FString& JoinedChannelName, const FVoiceChatResult& Result)
	{
		if (Result.IsSuccess())
		{
			UE_LOG(LogTemp, Log, TEXT("UserList: Successfully joined voice channel: %s"), *JoinedChannelName);

			// 송신 시작 (마이크 활성화)
			if (VoiceChatUser)
			{
				VoiceChatUser->SetChannelPlayerMuted(JoinedChannelName, VoiceChatUser->GetLoggedInPlayerName(), false);
				VoiceChatUser->SetAudioInputDeviceMuted(false);
				VoiceChatUser->SetAudioOutputDeviceMuted(false);

				UE_LOG(LogTemp, Log, TEXT("UserList: Audio input/output enabled"));
			}

			bIsVoiceChatActive = true;

			// 버튼 상태 업데이트
			if (StartButton)
			{
				StartButton->SetIsEnabled(false);
			}
			if (StopButton)
			{
				StopButton->SetIsEnabled(true);
			}

			// 화면에 메시지 출력
			if (GEngine)
			{
				GEngine->AddOnScreenDebugMessage(-1, 3.0f, FColor::Green,
					FString::Printf(TEXT("Voice Chat Connected: %s"), *JoinedChannelName));
			}
		}
		else
		{
			UE_LOG(LogTemp, Error, TEXT("UserList: Failed to join voice channel: %s"), *Result.ErrorDesc);

			// 화면에 에러 메시지 출력
			if (GEngine)
			{
				GEngine->AddOnScreenDebugMessage(-1, 3.0f, FColor::Red,
					FString::Printf(TEXT("Voice Chat Failed: %s"), *Result.ErrorDesc));
			}
		}
	}));
}

void UUserList::DisconnectFromVoiceChannel()
{
	if (!VoiceChatUser)
	{
		UE_LOG(LogTemp, Warning, TEXT("UserList: VoiceChatUser is null, cannot disconnect"));
		return;
	}

	UE_LOG(LogTemp, Log, TEXT("UserList: Disconnecting from voice channel: %s"), *ChannelName);

	// 채널에서 나가기
	VoiceChatUser->LeaveChannel(ChannelName,
		FOnVoiceChatChannelLeaveCompleteDelegate::CreateLambda([this](const FString& LeftChannelName, const FVoiceChatResult& Result)
	{
		if (Result.IsSuccess())
		{
			UE_LOG(LogTemp, Log, TEXT("UserList: Successfully left voice channel: %s"), *LeftChannelName);

			bIsVoiceChatActive = false;

			// 버튼 상태 업데이트
			if (StopButton)
			{
				StopButton->SetIsEnabled(false);
			}
			if (StartButton)
			{
				StartButton->SetIsEnabled(true);
			}

			// 화면에 메시지 출력
			if (GEngine)
			{
				GEngine->AddOnScreenDebugMessage(-1, 3.0f, FColor::Yellow, TEXT("Voice Chat Disconnected"));
			}
		}
		else
		{
			UE_LOG(LogTemp, Error, TEXT("UserList: Failed to leave voice channel: %s"), *Result.ErrorDesc);
		}
	}));
}
