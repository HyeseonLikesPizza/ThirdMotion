// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerState.h"
#include "ThirdMotionPlayerState.generated.h"

/**
 * 
 */
UCLASS()
class THIRDMOTION_API AThirdMotionPlayerState : public APlayerState
{
	GENERATED_BODY()

	//서버에게 채팅 내용 전달 RPC
	UFUNCTION(Server,Reliable)
	void ServerRPC_SendChat(const FString& chat);
	//모든 클라에게 채팅 내용 정달 RPC
	UFUNCTION(NetMulticast,Reliable)
	void MulticastRPC_SendChat(const FString& chat);
	
};
