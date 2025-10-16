// Fill out your copyright notice in the Description page of Project Settings.

#include "Framework/ThirdMotionGameMode.h"
#include "Framework/ThirdMotionPlayerController.h"
#include "Blueprint/UserWidget.h"

AThirdMotionGameMode::AThirdMotionGameMode()
{
	// Set default PlayerController class
	PlayerControllerClass = AThirdMotionPlayerController::StaticClass();

}

void AThirdMotionGameMode::BeginPlay()
{
	Super::BeginPlay();
}
