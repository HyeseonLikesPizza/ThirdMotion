// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "MaterialDetailsPanel.generated.h"

/**
 * 
 */
UCLASS()
class THIRDMOTION_API UMaterialDetailsPanel : public UUserWidget
{
	GENERATED_BODY()


public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta=(BindWidget))
	class UTextBlock* MaterialNameText;


	//메터리얼 타일뷰가 클릭되면 오른쪽 디테일창이 켜지는 함수.( 이름과 Peview image를 넘겨받는다)
	UFUNCTION()
	void SetMaterialNameAndImage(FText name, UMaterial* mat);
};
