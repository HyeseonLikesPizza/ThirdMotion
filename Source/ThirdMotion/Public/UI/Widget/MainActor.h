// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "MainActor.generated.h"

UCLASS()
class THIRDMOTION_API AMainActor : public AActor
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	AMainActor();

	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "UI")
	TSubclassOf<class UMainWidget> mainWidget;
	UPROPERTY()
	class UMainWidget* mainUI;
	
};
