// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/Widget/MainActor.h"
#include "Blueprint/UserWidget.h"
#include "UI/Widget/MainWidget.h"

// Sets default values
AMainActor::AMainActor()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;
}

// Called when the game starts or when spawned
void AMainActor::BeginPlay()
{
	Super::BeginPlay();
	
	mainUI = CreateWidget<UMainWidget>(GetWorld(), mainWidget);
	mainUI->AddToViewport();
	
	
}
