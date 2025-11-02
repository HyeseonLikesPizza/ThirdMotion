// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "VoiceChatManager.generated.h"

class IVoiceChat;

/**
 * VoiceChatManager - Voice Chat Plugin 직접 사용 (세션 미사용)
 * EOS Voice Chat을 사용하여 음성 채팅 구현
 */
UCLASS(BlueprintType)
class THIRDMOTION_API UVoiceChatManager : public UObject
{
	GENERATED_BODY()

public:
	// 초기화
	void Initialize();

	// 음성 채팅 로그인
	UFUNCTION(BlueprintCallable, Category = "Voice Chat")
	void LoginToVoiceChat(const FString& PlayerName);

	// 채널 입장
	UFUNCTION(BlueprintCallable, Category = "Voice Chat")
	void JoinChannel(const FString& ChannelName);

	// 채널 퇴장
	UFUNCTION(BlueprintCallable, Category = "Voice Chat")
	void LeaveChannel();

	// 음소거 토글
	UFUNCTION(BlueprintCallable, Category = "Voice Chat")
	void ToggleMute();

	// 마이크 볼륨 설정 (0.0 ~ 1.0)
	UFUNCTION(BlueprintCallable, Category = "Voice Chat")
	void SetMicrophoneVolume(float Volume);

	// 스피커 볼륨 설정 (0.0 ~ 1.0)
	UFUNCTION(BlueprintCallable, Category = "Voice Chat")
	void SetSpeakerVolume(float Volume);

	// 음소거 상태 확인
	UFUNCTION(BlueprintPure, Category = "Voice Chat")
	bool IsMuted() const { return bIsMuted; }

	// 채널 입장 상태 확인
	UFUNCTION(BlueprintPure, Category = "Voice Chat")
	bool IsInChannel() const { return bIsInChannel; }

	// 로그인 상태 확인
	UFUNCTION(BlueprintPure, Category = "Voice Chat")
	bool IsLoggedIn() const { return bIsLoggedIn; }

	// 정리
	void Shutdown();

	// 델리게이트: 로그인 완료
	DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnVoiceChatLoginComplete, bool, bSuccess);
	UPROPERTY(BlueprintAssignable, Category = "Voice Chat")
	FOnVoiceChatLoginComplete OnLoginComplete;

	// 델리게이트: 채널 입장 완료
	DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnVoiceChatChannelJoined, const FString&, ChannelName, bool, bSuccess);
	UPROPERTY(BlueprintAssignable, Category = "Voice Chat")
	FOnVoiceChatChannelJoined OnChannelJoined;

	// 델리게이트: 음소거 상태 변경
	DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnVoiceChatMuteChanged, bool, bMuted);
	UPROPERTY(BlueprintAssignable, Category = "Voice Chat")
	FOnVoiceChatMuteChanged OnMuteChanged;

private:
	// Voice Chat 인터페이스
	TSharedPtr<IVoiceChat> VoiceChat;

	// 현재 채널 이름
	FString CurrentChannelName;

	// 플레이어 이름
	FString PlayerName;

	// 음소거 상태
	bool bIsMuted = false;

	// 채널 입장 상태
	bool bIsInChannel = false;

	// 로그인 상태
	bool bIsLoggedIn = false;

	// 콜백 함수들
	void OnLoginCompleteCallback(const FString& InPlayerName, bool bSuccess);
	void OnChannelJoinedCallback(const FString& ChannelName, bool bSuccess);
};
