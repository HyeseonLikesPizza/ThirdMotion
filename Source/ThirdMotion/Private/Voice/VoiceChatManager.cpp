// Fill out your copyright notice in the Description page of Project Settings.

#include "Voice/VoiceChatManager.h"
#include "VoiceChat.h"
#include "Features/IModularFeatures.h"

void UVoiceChatManager::Initialize()
{
	// Voice Chat 인터페이스 가져오기
	if (IModularFeatures::Get().IsModularFeatureAvailable(TEXT("VoiceChat")))
	{
		VoiceChat = IModularFeatures::Get().GetModularFeature<IVoiceChat>(TEXT("VoiceChat"));

		if (VoiceChat.IsValid())
		{
			UE_LOG(LogTemp, Log, TEXT("VoiceChatManager: VoiceChat interface initialized"));

			// Voice Chat 초기화
			if (!VoiceChat->Initialize())
			{
				UE_LOG(LogTemp, Error, TEXT("VoiceChatManager: Failed to initialize VoiceChat"));
				VoiceChat.Reset();
			}
		}
		else
		{
			UE_LOG(LogTemp, Error, TEXT("VoiceChatManager: VoiceChat feature not available"));
		}
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("VoiceChatManager: VoiceChat modular feature not registered"));
	}
}

void UVoiceChatManager::LoginToVoiceChat(const FString& InPlayerName)
{
	if (!VoiceChat.IsValid())
	{
		UE_LOG(LogTemp, Error, TEXT("VoiceChatManager: VoiceChat is not initialized"));
		OnLoginComplete.Broadcast(false);
		return;
	}

	PlayerName = InPlayerName;

	UE_LOG(LogTemp, Log, TEXT("VoiceChatManager: Logging in with PlayerName: %s"), *PlayerName);

	// 로그인 옵션 설정
	FVoiceChatLoginOptions LoginOptions;
	LoginOptions.PlayerName = PlayerName;

	// 로그인 시작
	VoiceChat->Login(
		0, // Platform User ID
		PlayerName,
		LoginOptions,
		FOnVoiceChatLoginCompleteDelegate::CreateLambda([this](const FString& LoggedInPlayerName, const FVoiceChatResult& Result)
		{
			OnLoginCompleteCallback(LoggedInPlayerName, Result.IsSuccess());
		})
	);
}

void UVoiceChatManager::OnLoginCompleteCallback(const FString& InPlayerName, bool bSuccess)
{
	bIsLoggedIn = bSuccess;

	if (bSuccess)
	{
		UE_LOG(LogTemp, Log, TEXT("VoiceChatManager: Login successful - Player: %s"), *InPlayerName);
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("VoiceChatManager: Login failed - Player: %s"), *InPlayerName);
	}

	OnLoginComplete.Broadcast(bSuccess);
}

void UVoiceChatManager::JoinChannel(const FString& ChannelName)
{
	if (!VoiceChat.IsValid() || !bIsLoggedIn)
	{
		UE_LOG(LogTemp, Error, TEXT("VoiceChatManager: Not logged in to VoiceChat"));
		OnChannelJoined.Broadcast(ChannelName, false);
		return;
	}

	CurrentChannelName = ChannelName;

	UE_LOG(LogTemp, Log, TEXT("VoiceChatManager: Joining channel: %s"), *ChannelName);

	// 채널 입장 옵션 설정
	FVoiceChatChannel3dProperties Properties;
	Properties.AttenuationModel = EVoiceChatAttenuationModel::None; // 3D 오디오 미사용
	Properties.Distance = 1000.0f;
	Properties.MinDistance = 100.0f;
	Properties.Rolloff = 1.0f;

	// 채널 입장
	VoiceChat->JoinChannel(
		PlayerName,
		ChannelName,
		EVoiceChatChannelType::NonPositional, // 2D 음성 채팅
		FOnVoiceChatChannelJoinCompleteDelegate::CreateLambda([this, ChannelName](const FString& JoinedChannelName, const FVoiceChatResult& Result)
		{
			OnChannelJoinedCallback(JoinedChannelName, Result.IsSuccess());
		}),
		Properties
	);
}

void UVoiceChatManager::OnChannelJoinedCallback(const FString& ChannelName, bool bSuccess)
{
	bIsInChannel = bSuccess;

	if (bSuccess)
	{
		UE_LOG(LogTemp, Log, TEXT("VoiceChatManager: Joined channel successfully - Channel: %s"), *ChannelName);
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("VoiceChatManager: Failed to join channel - Channel: %s"), *ChannelName);
	}

	OnChannelJoined.Broadcast(ChannelName, bSuccess);
}

void UVoiceChatManager::LeaveChannel()
{
	if (!VoiceChat.IsValid() || CurrentChannelName.IsEmpty())
	{
		UE_LOG(LogTemp, Warning, TEXT("VoiceChatManager: No channel to leave"));
		return;
	}

	UE_LOG(LogTemp, Log, TEXT("VoiceChatManager: Leaving channel: %s"), *CurrentChannelName);

	VoiceChat->LeaveChannel(
		PlayerName,
		CurrentChannelName,
		FOnVoiceChatChannelLeaveCompleteDelegate::CreateLambda([this](const FString& LeftChannelName, const FVoiceChatResult& Result)
		{
			if (Result.IsSuccess())
			{
				UE_LOG(LogTemp, Log, TEXT("VoiceChatManager: Left channel successfully - Channel: %s"), *LeftChannelName);
				bIsInChannel = false;
			}
			else
			{
				UE_LOG(LogTemp, Error, TEXT("VoiceChatManager: Failed to leave channel - Channel: %s"), *LeftChannelName);
			}
		})
	);

	CurrentChannelName.Empty();
}

void UVoiceChatManager::ToggleMute()
{
	if (!VoiceChat.IsValid())
	{
		UE_LOG(LogTemp, Error, TEXT("VoiceChatManager: VoiceChat is not initialized"));
		return;
	}

	bIsMuted = !bIsMuted;

	// 입력 음소거 설정
	VoiceChat->SetAudioInputDeviceMuted(bIsMuted);

	UE_LOG(LogTemp, Log, TEXT("VoiceChatManager: Mute toggled - Muted: %d"), bIsMuted);

	OnMuteChanged.Broadcast(bIsMuted);
}

void UVoiceChatManager::SetMicrophoneVolume(float Volume)
{
	if (!VoiceChat.IsValid())
	{
		UE_LOG(LogTemp, Error, TEXT("VoiceChatManager: VoiceChat is not initialized"));
		return;
	}

	// 볼륨 범위 제한 (0.0 ~ 1.0)
	Volume = FMath::Clamp(Volume, 0.0f, 1.0f);

	VoiceChat->SetAudioInputVolume(Volume);

	UE_LOG(LogTemp, Log, TEXT("VoiceChatManager: Microphone volume set to %f"), Volume);
}

void UVoiceChatManager::SetSpeakerVolume(float Volume)
{
	if (!VoiceChat.IsValid())
	{
		UE_LOG(LogTemp, Error, TEXT("VoiceChatManager: VoiceChat is not initialized"));
		return;
	}

	// 볼륨 범위 제한 (0.0 ~ 1.0)
	Volume = FMath::Clamp(Volume, 0.0f, 1.0f);

	VoiceChat->SetAudioOutputVolume(Volume);

	UE_LOG(LogTemp, Log, TEXT("VoiceChatManager: Speaker volume set to %f"), Volume);
}

void UVoiceChatManager::Shutdown()
{
	if (VoiceChat.IsValid())
	{
		// 채널 퇴장
		if (bIsInChannel)
		{
			LeaveChannel();
		}

		// 로그아웃
		if (bIsLoggedIn)
		{
			VoiceChat->Logout(PlayerName);
		}

		// Voice Chat 정리
		VoiceChat->Uninitialize();
		VoiceChat.Reset();

		UE_LOG(LogTemp, Log, TEXT("VoiceChatManager: Shutdown complete"));
	}

	bIsLoggedIn = false;
	bIsInChannel = false;
	bIsMuted = false;
}
