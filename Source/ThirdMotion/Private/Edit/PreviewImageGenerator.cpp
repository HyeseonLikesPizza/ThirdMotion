// Fill out your copyright notice in the Description page of Project Settings.


#include "Edit/PreviewImageGenerator.h"


// Sets default values
APreviewImageGenerator::APreviewImageGenerator()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
}

// Called when the game starts or when spawned
void APreviewImageGenerator::BeginPlay()
{
	Super::BeginPlay();

}

// Called every frame
void APreviewImageGenerator::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void APreviewImageGenerator::ApplyMaterialToSphere(EMaterialType eMatType)
{
	switch (eMatType)
	{
	case EMaterialType::Standard:
		break;
	}
	
	
}

