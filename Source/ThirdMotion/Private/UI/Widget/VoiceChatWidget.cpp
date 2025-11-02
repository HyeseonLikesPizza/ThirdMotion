// Fill out your copyright notice in the Description page of Project Settings.

#include "UI/Widget/VoiceChatWidget.h"
#include "Components/Button.h"
#include "Components/Slider.h"
#include "Components/TextBlock.h"
#include "Voice/VoiceChatManager.h"

void UVoiceChatWidget::NativeConstruct()
{
	Super::NativeConstruct();

	// 버튼 바인딩
	if (MuteButton)
	{
		MuteButton->OnClicked.AddDynamic(this, &UVoiceChatWidget::OnMuteButtonClicked);
	}

	// 슬라이더 바인딩
	if (MicVolumeSlider)
	{
		MicVolumeSlider->SetValue(1.0f); // 기본값 100%
		MicVolumeSlider->OnValueChanged.AddDynamic(this, &UVoiceChatWidget::OnMicVolumeChanged);
	}

	if (SpeakerVolumeSlider)
	{
		SpeakerVolumeSlider->SetValue(1.0f); // 기본값 100%
		SpeakerVolumeSlider->OnValueChanged.AddDynamic(this, &UVoiceChatWidget::OnSpeakerVolumeChanged);
	}

	// 초기 UI 업데이트
	UpdateMuteStatusUI();
	UpdateChannelStatusUI();

	UE_LOG(LogTemp, Log, TEXT("VoiceChatWidget: NativeConstruct complete"));
}

void UVoiceChatWidget::NativeDestruct()
{
	// 이벤트 언바인딩
	if (MuteButton)
	{
		MuteButton->OnClicked.RemoveDynamic(this, &UVoiceChatWidget::OnMuteButtonClicked);
	}

	if (MicVolumeSlider)
	{
		MicVolumeSlider->OnValueChanged.RemoveDynamic(this, &UVoiceChatWidget::OnMicVolumeChanged);
	}

	if (SpeakerVolumeSlider)
	{
		SpeakerVolumeSlider->OnValueChanged.RemoveDynamic(this, &UVoiceChatWidget::OnSpeakerVolumeChanged);
	}

	// VoiceChatManager 이벤트 언바인딩
	if (VoiceChatManager)
	{
		VoiceChatManager->OnLoginComplete.RemoveDynamic(this, &UVoiceChatWidget::OnLoginComplete);
		VoiceChatManager->OnChannelJoined.RemoveDynamic(this, &UVoiceChatWidget::OnChannelJoined);
		VoiceChatManager->OnMuteChanged.RemoveDynamic(this, &UVoiceChatWidget::OnMuteChanged);
	}

	Super::NativeDestruct();
}

void UVoiceChatWidget::SetVoiceChatManager(UVoiceChatManager* InManager)
{
	VoiceChatManager = InManager;

	if (VoiceChatManager)
	{
		// 델리게이트 바인딩
		VoiceChatManager->OnLoginComplete.AddDynamic(this, &UVoiceChatWidget::OnLoginComplete);
		VoiceChatManager->OnChannelJoined.AddDynamic(this, &UVoiceChatWidget::OnChannelJoined);
		VoiceChatManager->OnMuteChanged.AddDynamic(this, &UVoiceChatWidget::OnMuteChanged);

		UE_LOG(LogTemp, Log, TEXT("VoiceChatWidget: VoiceChatManager set and delegates bound"));
	}
}

void UVoiceChatWidget::OnMuteButtonClicked()
{
	if (VoiceChatManager)
	{
		VoiceChatManager->ToggleMute();
		UE_LOG(LogTemp, Log, TEXT("VoiceChatWidget: Mute button clicked"));
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("VoiceChatWidget: VoiceChatManager is null"));
	}
}

void UVoiceChatWidget::OnMicVolumeChanged(float Value)
{
	if (VoiceChatManager)
	{
		VoiceChatManager->SetMicrophoneVolume(Value);
		UE_LOG(LogTemp, Log, TEXT("VoiceChatWidget: Mic volume changed to %f"), Value);
	}
}

void UVoiceChatWidget::OnSpeakerVolumeChanged(float Value)
{
	if (VoiceChatManager)
	{
		VoiceChatManager->SetSpeakerVolume(Value);
		UE_LOG(LogTemp, Log, TEXT("VoiceChatWidget: Speaker volume changed to %f"), Value);
	}
}

void UVoiceChatWidget::OnLoginComplete(bool bSuccess)
{
	if (bSuccess)
	{
		UE_LOG(LogTemp, Log, TEXT("VoiceChatWidget: Login successful"));
		UpdateChannelStatusUI();
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("VoiceChatWidget: Login failed"));
		if (ChannelStatusText)
		{
			ChannelStatusText->SetText(FText::FromString(TEXT("Login Failed")));
		}
	}
}

void UVoiceChatWidget::OnChannelJoined(const FString& ChannelName, bool bSuccess)
{
	if (bSuccess)
	{
		UE_LOG(LogTemp, Log, TEXT("VoiceChatWidget: Joined channel: %s"), *ChannelName);
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("VoiceChatWidget: Failed to join channel: %s"), *ChannelName);
	}

	UpdateChannelStatusUI();
}

void UVoiceChatWidget::OnMuteChanged(bool bMuted)
{
	UE_LOG(LogTemp, Log, TEXT("VoiceChatWidget: Mute status changed to %d"), bMuted);
	UpdateMuteStatusUI();
}

void UVoiceChatWidget::UpdateMuteStatusUI()
{
	if (!MuteStatusText || !VoiceChatManager)
		return;

	if (VoiceChatManager->IsMuted())
	{
		MuteStatusText->SetText(FText::FromString(TEXT("Muted")));
	}
	else
	{
		MuteStatusText->SetText(FText::FromString(TEXT("Unmuted")));
	}
}

void UVoiceChatWidget::UpdateChannelStatusUI()
{
	if (!ChannelStatusText || !VoiceChatManager)
		return;

	if (VoiceChatManager->IsInChannel())
	{
		ChannelStatusText->SetText(FText::FromString(TEXT("In Channel")));
	}
	else if (VoiceChatManager->IsLoggedIn())
	{
		ChannelStatusText->SetText(FText::FromString(TEXT("Logged In")));
	}
	else
	{
		ChannelStatusText->SetText(FText::FromString(TEXT("Not Connected")));
	}
}
