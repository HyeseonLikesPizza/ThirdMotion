// Fill out your copyright notice in the Description page of Project Settings.


#include "Framework/ThirdMotionPlayerState.h"

void AThirdMotionPlayerState::ServerRPC_SendChat_Implementation(const FString& chat)
{
	//채팅 내용을 --->이름 : 채팅내용
	FString sendChat = FString::Printf(TEXT("%s : %s"), *GetPlayerName(), *chat);
	//모든 클라에게 sendChat을 보내자.
	MulticastRPC_SendChat(sendChat);
}

void AThirdMotionPlayerState::MulticastRPC_SendChat_Implementation(const FString& chat)
{
	//GameState 가져오자
	// ANetGameState* gs = Cast<ANetGameState>(GetWorld()->GetGameState());
}
