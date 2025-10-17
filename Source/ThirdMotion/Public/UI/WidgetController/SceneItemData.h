// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "SceneItemData.generated.h"

/**
 * 
 */
UCLASS()
class THIRDMOTION_API USceneItemData : public UObject
{
	GENERATED_BODY()

public:

	UPROPERTY(BlueprintReadWrite)
	AActor* Actor;

	UPROPERTY(BlueprintReadWrite)
	FString DisplayName;

	UPROPERTY(BlueprintReadWrite)
	TArray<USceneItemData*> Children;

	UPROPERTY(BlueprintReadWrite)
	bool bIsVisible = true;

	UPROPERTY(BlueprintReadWrite)
	UTexture2D* Icon;
	
};
