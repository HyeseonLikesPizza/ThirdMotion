// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UI/Widget/BaseWidget.h"
#include "Components/TextBlock.h"
#include "Components/Button.h"
#include "Components/Image.h"
#include "PropertyRow.generated.h"

class UActorDetailsController;

/**
 * PropertyRow - Widget (MVC Pattern의 View)
 * Details 패널의 각 속성을 표시하는 행 위젯
 * Property 이름과 값을 표시하고, 클릭 시 Controller를 통해 액션 수행
 */
UCLASS()
class THIRDMOTION_API UPropertyRow : public UBaseWidget
{
	GENERATED_BODY()

public:
	virtual void NativeConstruct() override;

	// Property 설정
	UFUNCTION(BlueprintCallable, Category = "Property Row")
	void SetPropertyName(const FString& InName);

	UFUNCTION(BlueprintCallable, Category = "Property Row")
	void SetPropertyValue(const FString& InValue);

	// Controller 설정
	UFUNCTION(BlueprintCallable, Category = "Property Row")
	void SetDetailsController(UActorDetailsController* InController);

	// Property 타입별 설정
	UFUNCTION(BlueprintCallable, Category = "Property Row")
	void SetupForMesh(UObject* CurrentMesh, UMeshComponent* TargetComponent);

	UFUNCTION(BlueprintCallable, Category = "Property Row")
	void SetupForMaterial(UObject* CurrentMaterial, UMeshComponent* TargetComponent, int32 MaterialIndex);

	// Widget Components
	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	UTextBlock* PropertyNameText;

	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	UTextBlock* PropertyValueText;

	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	UButton* ValueButton;

	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	UImage* ThumbnailImage;

protected:
	UPROPERTY()
	FString PropertyName;

	UPROPERTY()
	FString PropertyValue;

	// Property 타입
	UPROPERTY()
	UClass* PropertyAssetClass;

	// Target Component (Mesh 또는 Material을 적용할 대상)
	UPROPERTY()
	UMeshComponent* TargetMeshComponent;

	UPROPERTY()
	int32 TargetMaterialIndex;

	UPROPERTY()
	bool bIsMaterialProperty;

	// Controller 참조
	UPROPERTY()
	UActorDetailsController* DetailsController;

private:
	UFUNCTION()
	void OnValueButtonClicked();

	// 썸네일 업데이트
	void UpdateThumbnail(UObject* Asset);
};
