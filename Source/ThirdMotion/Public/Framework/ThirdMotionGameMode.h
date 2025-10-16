// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameMode.h"
#include "ThirdMotionGameMode.generated.h"

/**
 * ThirdMotion GameMode
 */
UCLASS()
class THIRDMOTION_API AThirdMotionGameMode : public AGameMode
{
	GENERATED_BODY()

public:
	AThirdMotionGameMode();

	virtual void BeginPlay() override;
};
