// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "ThirdMotionGameInstance.generated.h"

/**
 * 
 */
UCLASS()
class THIRDMOTION_API UThirdMotionGameInstance : public UGameInstance
{
	GENERATED_BODY()

	virtual void Init() override;

	UFUNCTION()
	void BeginLoadingScreen(const FString& MapName);

	UFUNCTION()
	void BeginLoadingScreen_WithContext(const FWorldContext& inWorldContext, const FString& MapName);

	UPROPERTY()
	TWeakObjectPtr<class UUserWidget> LoadingUMG;
	
};
