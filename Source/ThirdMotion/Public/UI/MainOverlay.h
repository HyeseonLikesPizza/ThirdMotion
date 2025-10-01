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
	UEditableTextBox* urlInput;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	UButton* joinBt;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	UButton* hostBt;

	// 내부 상태
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Network")
	FString url;

private:
	// 버튼/입력 핸들러
	UFUNCTION()
	void CreateHost();

	UFUNCTION()
	void ClickJoinButton();

	UFUNCTION()
	void joinUrl(const FText& inText, ETextCommit::Type inCommitMethod);

	UFUNCTION()
	void InputUrl(const FText& changedText);
	
};
