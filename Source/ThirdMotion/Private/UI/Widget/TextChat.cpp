// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/Widget/TextChat.h"

#include "Components/EditableTextBox.h"
#include "Components/ScrollBox.h"
#include "UI/Widget/ChatWidget.h"

void UTextChat::NativeConstruct()
{
	Super::NativeConstruct();

	//채팅 입력 후 엔터쳤을 때 호출되는 함수 등록
	editChat->OnTextCommitted.AddDynamic(this, &UTextChat::OnTextBoxCommitted);
	
}

void UTextChat::OnTextBoxCommitted(const FText& text, ETextCommit::Type commitMethod)
{
	//만약에 엔터를 친 이벤트면
	if (commitMethod == ETextCommit::OnEnter)
	{
		//채팅 UI 추가
		AddChat(text.ToString());
	}
	else if (commitMethod == ETextCommit::OnCleared)
	{
		//강제로 editChat 을 활성화
		editChat->SetFocus();
		
	}
}

void UTextChat::AddChat(FString text)
{
	//현재 스크롤 위치 값
	float scrollOffset = scrollChat->GetScrollOffset();
	//스크롤이 맨 끝일때 값
	float scrollOffsetOfEnd = scrollChat->GetScrollOffsetOfEnd();
	UE_LOG(LogTemp, Warning, TEXT("scroll : %f, %f"), scrollOffset, scrollOffsetOfEnd)
	
	//채팅 UI 만들어서 채팅 내용에 추가
	UChatWidget* chat = CreateWidget<UChatWidget>(GetWorld(), chatWidget);
	scrollChat->AddChild(chat);
	chat->SetContent(text);
	//editChat에 남아있는 내용 초기ㅗ하
	editChat->SetText(FText());

	//만약에 스크롤이 위치가 맨 끝이라면
	if (scrollOffset == scrollOffsetOfEnd)
	{
		//개행되는 채팅이 추가되면 한줄로 크기를 인식해서 발생하는 문제 때문에
		//ScrollToTend를 0.01초 뒤에 실행
		FTimerHandle handle;
		GetWorld()->GetTimerManager().SetTimer(handle,[this]()
		{
			//스크롤 위치를 맨 끝으로 해라!
			scrollChat->ScrollToEnd();
		},0.01f, false);
		//스크롤 위치를 맨 끝으로 해라!
		scrollChat->ScrollToEnd();
	}
}
