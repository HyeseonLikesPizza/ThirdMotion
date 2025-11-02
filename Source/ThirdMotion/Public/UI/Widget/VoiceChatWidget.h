// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "VoiceChatWidget.generated.h"

class UButton;
class USlider;
class UTextBlock;
class UVoiceChatManager;

/**
 * VoiceChatWidget - 음성 채팅 UI
 * 마이크 음소거, 볼륨 조절 등 제공
 */
UCLASS()
class THIRDMOTION_API UVoiceChatWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	virtual void NativeConstruct() override;
	virtual void NativeDestruct() override;

	// VoiceChatManager 설정
	void SetVoiceChatManager(UVoiceChatManager* InManager);

	// ==================== UI 위젯 바인딩 ====================

	// 음소거 버튼
	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	UButton* MuteButton;

	// 마이크 볼륨 슬라이더
	UPROPERTY(BlueprintReadWrite, meta = (BindWidgetOptional))
	USlider* MicVolumeSlider;

	// 스피커 볼륨 슬라이더
	UPROPERTY(BlueprintReadWrite, meta = (BindWidgetOptional))
	USlider* SpeakerVolumeSlider;

	// 음소거 상태 텍스트
	UPROPERTY(BlueprintReadWrite, meta = (BindWidgetOptional))
	UTextBlock* MuteStatusText;

	// 채널 상태 텍스트
	UPROPERTY(BlueprintReadWrite, meta = (BindWidgetOptional))
	UTextBlock* ChannelStatusText;

protected:
	// 버튼 클릭 핸들러
	UFUNCTION()
	void OnMuteButtonClicked();

	// 슬라이더 값 변경 핸들러
	UFUNCTION()
	void OnMicVolumeChanged(float Value);

	UFUNCTION()
	void OnSpeakerVolumeChanged(float Value);

	// VoiceChatManager 이벤트 핸들러
	UFUNCTION()
	void OnLoginComplete(bool bSuccess);

	UFUNCTION()
	void OnChannelJoined(const FString& ChannelName, bool bSuccess);

	UFUNCTION()
	void OnMuteChanged(bool bMuted);

private:
	// VoiceChatManager 참조
	UPROPERTY()
	UVoiceChatManager* VoiceChatManager;

	// UI 업데이트
	void UpdateMuteStatusUI();
	void UpdateChannelStatusUI();
};
