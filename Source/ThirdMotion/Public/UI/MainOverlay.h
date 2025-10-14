// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "MainOverlay.generated.h"

class UEditableTextBox;
class UButton;

/**
 * 
 */
UCLASS()
class THIRDMOTION_API UMainOverlay : public UUserWidget
{
	GENERATED_BODY()

public:
	virtual void NativeOnInitialized() override;
	virtual void NativeConstruct() override;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	UEditableTextBox* UrlInputBox;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	UButton* joinBt;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	UButton* hostBt;

	// 내부 상태
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Network")
	FString url;

	UPROPERTY(meta=(BindWidget))
	class UWidgetSwitcher* WidgetSwitcher;

	UPROPERTY(meta=(BindWidget))
	class UButton* JoinBtn;

	UPROPERTY(meta=(BindWidget))
	class UButton* HostBtn;

	UPROPERTY(meta=(BindWidget))
	class UButton* CancelBtn;

	UPROPERTY(meta=(BindWidget))
	class UButton* JoinConfirmBtn;

	
	
private:


	// 버튼/입력 핸들러
	UFUNCTION()
	void CreateHost();

	UFUNCTION()
	void GoToFirstPage();

	//Join 버튼을 누르면 Ip입력과 Confirm이 뜨는 창으로 이동
	UFUNCTION()
	void GoToJoinPage();

	UFUNCTION()
	void OnJoinConfirmClicked();

	
	// 서버 접속 실패 시 호출될 함수
	UFUNCTION()
	void HandleNetworkFailure(UWorld* World, UNetDriver* NetDriver, ENetworkFailure::Type FailureType, const FString& ErrorString);
	
};
