// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/Widget/PawnForTextChat.h"


// Sets default values
APawnForTextChat::APawnForTextChat()
{
	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
}

// Called when the game starts or when spawned
void APawnForTextChat::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void APawnForTextChat::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

// Called to bind functionality to input
void APawnForTextChat::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
}

