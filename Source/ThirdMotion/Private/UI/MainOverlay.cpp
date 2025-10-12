// Fill out your copyright notice in the Description page of Project Settings.


#include "ThirdMotion/Public/UI/MainOverlay.h"

#include "SocketSubsystem.h"
#include "Components/VerticalBox.h"
#include "Components/Button.h"
#include "Components/CanvasPanel.h"
#include "Components/EditableTextBox.h"
#include "Kismet/GameplayStatics.h"


void UMainOverlay::NativeConstruct()
{
	Super::NativeConstruct();
	urlInput->SetVisibility(ESlateVisibility::Hidden);
	joinBt->OnClicked.AddDynamic(this, &UMainOverlay::ClickJoinButton);
	urlInput->OnTextCommitted.AddDynamic(this, &UMainOverlay::joinUrl);
	hostBt->OnClicked.AddDynamic(this, &UMainOverlay::CreateHost);
}

void UMainOverlay::NativeOnInitialized()
{
	Super::NativeOnInitialized();
}

void UMainOverlay::CreateHost()
{
	bool bCanBindAll;
	TSharedPtr<FInternetAddr> LocalAddr = ISocketSubsystem::Get(PLATFORM_SOCKETSUBSYSTEM)->GetLocalHostAddr(*GLog, bCanBindAll);
    
	url = LocalAddr.IsValid() ? LocalAddr->ToString(false) : "127.0.0.1";
	
	UE_LOG(LogTemp, Display, TEXT("Host URL : %s"), *url);

	UGameplayStatics::OpenLevel(this, FName("MainMap"), true, "listen");
}

void UMainOverlay::ClickJoinButton()
{
	urlInput->SetVisibility(ESlateVisibility::Visible);
}

void UMainOverlay::joinUrl(const FText& inText, ETextCommit::Type inCommitMethod)
{
	if (inCommitMethod != ETextCommit::OnEnter)
	{
		return;
	}
	//inText 에 해당하는 url 검증 과정 필요
	UGameplayStatics::OpenLevel(this, *inText.ToString(), true);
}

void UMainOverlay::InputUrl(const FText& changedText)
{
}