// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "UI/Panel/MaterialGeneratePanel.h"
#include "PreviewImageGenerator.generated.h"

UCLASS()
class THIRDMOTION_API APreviewImageGenerator : public AActor
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	APreviewImageGenerator();
	

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	UStaticMeshComponent* PreviewSphere;


public:
	//인자값을 하나 받아서 MatType을 판단한 후 MatType에 맞는 기본 메터리얼을 설정
	void ApplyMaterialToSphere(EMaterialType eMatType);
	//프리뷰 이미지 캡쳐 후 텍스쳐 생성
	void CaptureAndGeneratePreviewImage();
	
	
};
