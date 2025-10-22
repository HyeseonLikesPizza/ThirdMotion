// Fill out your copyright notice in the Description page of Project Settings.

#include "UI/Widget/PropertyRow.h"
#include "UI/WidgetController/ActorDetailsController.h"
#include "Components/TextBlock.h"
#include "Components/Button.h"
#include "Components/Image.h"
#include "Engine/StaticMesh.h"
#include "Engine/SkeletalMesh.h"
#include "Materials/MaterialInterface.h"

void UPropertyRow::NativeConstruct()
{
	Super::NativeConstruct();

	// 버튼 클릭 이벤트 바인딩
	if (ValueButton)
	{
		ValueButton->OnClicked.AddDynamic(this, &UPropertyRow::OnValueButtonClicked);
	}

	bIsMaterialProperty = false;
	TargetMaterialIndex = 0;
}

void UPropertyRow::SetPropertyName(const FString& InName)
{
	PropertyName = InName;

	if (PropertyNameText)
	{
		PropertyNameText->SetText(FText::FromString(PropertyName));
	}
}

void UPropertyRow::SetPropertyValue(const FString& InValue)
{
	PropertyValue = InValue;

	if (PropertyValueText)
	{
		PropertyValueText->SetText(FText::FromString(PropertyValue));
	}
}

void UPropertyRow::SetDetailsController(UActorDetailsController* InController)
{
	DetailsController = InController;
}

void UPropertyRow::SetupForMesh(UObject* CurrentMesh, UMeshComponent* TargetComponent)
{
	TargetMeshComponent = TargetComponent;
	bIsMaterialProperty = false;

	// Mesh 타입에 따라 AssetClass 설정
	if (Cast<UStaticMeshComponent>(TargetComponent))
	{
		PropertyAssetClass = UStaticMesh::StaticClass();
	}
	else if (Cast<USkeletalMeshComponent>(TargetComponent))
	{
		PropertyAssetClass = USkeletalMesh::StaticClass();
	}

	// 썸네일 업데이트
	UpdateThumbnail(CurrentMesh);
}

void UPropertyRow::SetupForMaterial(UObject* CurrentMaterial, UMeshComponent* TargetComponent, int32 MaterialIndex)
{
	TargetMeshComponent = TargetComponent;
	TargetMaterialIndex = MaterialIndex;
	bIsMaterialProperty = true;
	PropertyAssetClass = UMaterialInterface::StaticClass();

	// 썸네일 업데이트
	UpdateThumbnail(CurrentMaterial);
}

void UPropertyRow::OnValueButtonClicked()
{
	UE_LOG(LogTemp, Log, TEXT("PropertyRow button clicked: %s"), *PropertyName);

	// TODO: RightPanel의 AssetPicker를 표시하도록 이벤트 발생
	// BottomBar의 Properties 버튼을 통해 AssetPicker를 표시할 예정

	// 임시로 로그만 출력
	UE_LOG(LogTemp, Warning, TEXT("AssetPicker should be shown in RightPanel"));
	UE_LOG(LogTemp, Warning, TEXT("PropertyAssetClass: %s"), PropertyAssetClass ? *PropertyAssetClass->GetName() : TEXT("None"));
	UE_LOG(LogTemp, Warning, TEXT("bIsMaterialProperty: %s"), bIsMaterialProperty ? TEXT("true") : TEXT("false"));
}

void UPropertyRow::UpdateThumbnail(UObject* Asset)
{
	if (!ThumbnailImage || !Asset)
		return;

	// 간단한 썸네일 표시
	// 실제 프로젝트에서는 AssetRegistry를 사용하여 썸네일을 가져올 수 있음

	UE_LOG(LogTemp, Log, TEXT("Thumbnail updated for: %s"), *Asset->GetName());
}
