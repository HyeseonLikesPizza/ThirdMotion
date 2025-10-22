// Fill out your copyright notice in the Description page of Project Settings.

#include "UI/Panel/ActorDetailsPanel.h"
#include "UI/Widget/PropertyRow.h"
#include "UI/WidgetController/ActorDetailsController.h"
#include "Data/ActorDetailsData.h"
#include "Components/ScrollBox.h"
#include "Components/TextBlock.h"

void UActorDetailsPanel::NativeConstruct()
{
	Super::NativeConstruct();

	// Controller와 Model 초기화
	InitializeDetailsData();
	InitializeController();

	// UI 초기화
	ClearUI();
}

void UActorDetailsPanel::NativeDestruct()
{
	// Observer 해제
	if (DetailsData)
	{
		DetailsData->OnDataChanged.RemoveDynamic(this, &UActorDetailsPanel::OnDetailsDataChanged);
	}

	Super::NativeDestruct();
}

void UActorDetailsPanel::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);
}

void UActorDetailsPanel::InitializeDetailsData()
{
	// ActorDetailsData 생성
	DetailsData = NewObject<UActorDetailsData>(this);

	if (DetailsData)
	{
		// Observer 등록: Model의 데이터 변경 알림 수신
		DetailsData->OnDataChanged.AddDynamic(this, &UActorDetailsPanel::OnDetailsDataChanged);

		UE_LOG(LogTemp, Log, TEXT("ActorDetailsData initialized"));
	}
}

void UActorDetailsPanel::InitializeController()
{
	// ActorDetailsController 생성
	DetailsController = NewObject<UActorDetailsController>(this);

	if (DetailsController)
	{
		// Controller 초기화
		DetailsController->Initialize(GetWorld(), DetailsData);

		UE_LOG(LogTemp, Log, TEXT("ActorDetailsController initialized"));
	}
}

void UActorDetailsPanel::OnDetailsDataChanged(UActorDetailsData* ChangedData)
{
	// Observer 패턴: Model의 데이터 변경 알림을 받아 UI 업데이트
	UE_LOG(LogTemp, Log, TEXT("ActorDetailsData changed, updating UI"));

	RefreshUI();
}

void UActorDetailsPanel::RefreshUI()
{
	// View의 역할: Model의 데이터를 UI에 표시
	ClearUI();

	if (!DetailsData)
		return;

	// 액터 이름 업데이트
	if (ActorNameText)
	{
		ActorNameText->SetText(FText::FromString(DetailsData->ActorName));
	}

	// 액터 클래스 업데이트
	if (ActorClassText)
	{
		ActorClassText->SetText(FText::FromString(DetailsData->ActorClassName));
	}

	// 속성 업데이트
	UpdateProperties();
}

void UActorDetailsPanel::ClearUI()
{
	// PropertyRow 위젯들 제거
	if (PropertiesScrollBox)
	{
		PropertiesScrollBox->ClearChildren();
	}

	PropertyRows.Empty();
}

void UActorDetailsPanel::UpdateProperties()
{
	if (!DetailsData || !PropertiesScrollBox || !PropertyRowClass)
		return;

	// Model의 Properties를 순회하며 UI 생성
	for (const FActorPropertyData& PropData : DetailsData->Properties)
	{
		UPropertyRow* Row = CreateWidget<UPropertyRow>(this, PropertyRowClass);
		if (Row)
		{
			// PropertyRow 설정
			Row->SetPropertyName(PropData.PropertyName);

			FString ValueText = PropData.PropertyValue ? PropData.PropertyValue->GetName() : TEXT("None");
			Row->SetPropertyValue(ValueText);

			// Controller 설정 (PropertyRow가 Controller를 통해 액션 수행)
			Row->SetDetailsController(DetailsController);

			// Property 데이터 설정
			if (PropData.bIsMaterialProperty)
			{
				Row->SetupForMaterial(PropData.PropertyValue, PropData.TargetComponent, PropData.MaterialIndex);
			}
			else
			{
				Row->SetupForMesh(PropData.PropertyValue, PropData.TargetComponent);
			}

			// ScrollBox에 추가
			PropertiesScrollBox->AddChild(Row);
			PropertyRows.Add(Row);
		}
	}

	UE_LOG(LogTemp, Log, TEXT("Updated %d property rows"), PropertyRows.Num());
}
