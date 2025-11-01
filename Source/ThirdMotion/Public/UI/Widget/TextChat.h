// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BaseWidget.h"
#include "TextChat.generated.h"

/**
 * 
 */
UCLASS()
class THIRDMOTION_API UTextChat : public UBaseWidget
{
	GENERATED_BODY()

	virtual void NativeConstruct() override;

	//채팅 내용 담고 있는 ScrollBox
	UPROPERTY(meta=(BindWidget))
	class UScrollBox* scrollChat;

	//채팅 내용 입력 하는 EditableTextBox;
	UPROPERTY(meta=(BindWidget))
	class UEditableTextBox* editChat;
	//채팅 입력 후 엔터를 쳤을 때 호출되는 함수
	UFUNCTION()
	void OnTextBoxCommitted(const FText& text, ETextCommit::Type commitMethod);

	//ChatWidget 블루프린트
	UPROPERTY(EditAnywhere)
	TSubclassOf<class UChatWidget> chatWidget;

	//채팅 UI 추가 함수
	void AddChat(FString text);
};
