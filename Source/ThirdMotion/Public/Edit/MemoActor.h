// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "MemoActor.generated.h"

class UWidgetComponent;

/**
 * MemoActor - 항상 카메라를 바라보는 메모 위젯 액터
 */
UCLASS()
class THIRDMOTION_API AMemoActor : public AActor
{
	GENERATED_BODY()

public:
	AMemoActor();

protected:
	virtual void BeginPlay() override;

public:
	virtual void Tick(float DeltaTime) override;

	// 위젯 컴포넌트 (WBP_ViewMemo)
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Memo")
	UWidgetComponent* MemoWidgetComponent;
};
