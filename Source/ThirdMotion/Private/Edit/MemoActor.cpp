// Fill out your copyright notice in the Description page of Project Settings.

#include "Edit/MemoActor.h"
#include "Components/WidgetComponent.h"
#include "Camera/PlayerCameraManager.h"
#include "Kismet/GameplayStatics.h"

AMemoActor::AMemoActor()
{
	PrimaryActorTick.bCanEverTick = true;

	// 루트 컴포넌트 생성 (SceneComponent)
	USceneComponent* Root = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
	SetRootComponent(Root);

	// WidgetComponent 생성
	MemoWidgetComponent = CreateDefaultSubobject<UWidgetComponent>(TEXT("MemoWidgetComponent"));
	MemoWidgetComponent->SetupAttachment(RootComponent);
	MemoWidgetComponent->SetWidgetSpace(EWidgetSpace::World);
	MemoWidgetComponent->SetDrawSize(FVector2D(300.0f, 300.0f));
}

void AMemoActor::BeginPlay()
{
	Super::BeginPlay();

	UE_LOG(LogTemp, Log, TEXT("MemoActor: BeginPlay"));
}

void AMemoActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	// 로컬 플레이어의 카메라 찾기
	APlayerCameraManager* CameraManager = UGameplayStatics::GetPlayerCameraManager(GetWorld(), 0);
	if (!CameraManager)
		return;

	// 카메라 위치
	FVector CameraLocation = CameraManager->GetCameraLocation();

	// 위젯이 카메라를 바라보도록 회전 설정
	if (MemoWidgetComponent)
	{
		FVector WidgetLocation = MemoWidgetComponent->GetComponentLocation();
		FRotator LookAtRotation = (CameraLocation - WidgetLocation).Rotation();

		// 위젯 컴포넌트의 회전 설정
		MemoWidgetComponent->SetWorldRotation(LookAtRotation);
	}
}
