// Fill out your copyright notice in the Description page of Project Settings.

#include "UI/WidgetController/RightPanelController.h"
#include "ThirdMotion/ThirdMotion.h"
#include "UI/Panel/RightPanel.h"
#include "UI/Widget/LightWidget.h"
#include "UI/Widget/Mesh/MeshSettingsWidget.h"
#include "UI/WidgetController/MeshWidgetController.h"
#include "Edit/AssetResolver.h"
#include "Framework/ThirdMotionPlayerController.h"
#include "Components/LightComponent.h"
#include "GameplayTagContainer.h"

void URightPanelController::InitializeWithRightPanel(URightPanel* InRightPanel)
{
	if (!InRightPanel)
	{
		UE_LOG(LogTemp, Warning, TEXT("RightPanelController: RightPanel is null"));
		return;
	}

	RightPanel = InRightPanel;
	CurrentPanel = ERightPanelType::Scene; // 기본값: Scene
	bIsPanelVisible = false; // 초기에는 숨김

	// RightPanel 초기 상태: 숨김
	RightPanel->SetRightPanelVisibility(false);

	// MeshWidgetController 생성 후 설정
	UMeshSettingsWidget* MeshWidget = RightPanel->MeshSettingsWidget;
	if (!MeshWidget)
	{
		PRINTLOG(TEXT("MeshWidget Invalid"));
		return;
	}

	UAssetResolver* Resolver = GetWorld()->GetSubsystem<UAssetResolver>();
	AThirdMotionPlayerController* PC = Cast<AThirdMotionPlayerController>(GetWorld()->GetFirstPlayerController());

	MeshWidgetController = NewObject<UMeshWidgetController>(this);
	MeshWidgetController->Initialize(Resolver);
	MeshWidgetController->AttachView(MeshWidget);
	MeshWidgetController->BindPlayerController(PC);

	UE_LOG(LogTemp, Log, TEXT("RightPanelController: Initialized with RightPanel (hidden)"));
}

void URightPanelController::SwitchToPanel(ERightPanelType PanelType)
{
	if (!RightPanel)
	{
		UE_LOG(LogTemp, Warning, TEXT("RightPanelController: RightPanel is null"));
		return;
	}

	// 패널 타입 업데이트
	CurrentPanel = PanelType;
	bIsPanelVisible = true;

	// RightPanel 보이기
	RightPanel->SetRightPanelVisibility(true);

	// RightPanel에 WidgetSwitcher 인덱스 변경 요청
	int32 PanelIndex = static_cast<int32>(PanelType);
	RightPanel->SetWidgetSwitcherIndex(PanelIndex);

	// 패널별 초기화
	switch (PanelType)
	{
	case ERightPanelType::Scene:
		InitializeScenePanel();
		break;
	case ERightPanelType::Properties:
		InitializePropertiesPanel();
		break;
	case ERightPanelType::UserList:
		InitializeUserListPanel();
		break;
	}

	UE_LOG(LogTemp, Log, TEXT("RightPanelController: Switched to panel %d"), PanelIndex);
}

void URightPanelController::SwitchToPanel(ERightPanelType PanelType, int32 CustomIndex)
{
	if (!RightPanel)
	{
		UE_LOG(LogTemp, Warning, TEXT("RightPanelController: RightPanel is null"));
		return;
	}

	// 패널 타입 업데이트
	CurrentPanel = PanelType;
	bIsPanelVisible = true;

	// RightPanel 보이기
	RightPanel->SetRightPanelVisibility(true);

	// RightPanel에 커스텀 WidgetSwitcher 인덱스 변경 요청
	RightPanel->SetWidgetSwitcherIndex(CustomIndex);

	// 패널별 초기화
	switch (PanelType)
	{
	case ERightPanelType::Scene:
		InitializeScenePanel();
		break;
	case ERightPanelType::Properties:
		InitializePropertiesPanel();
		break;
	case ERightPanelType::UserList:
		InitializeUserListPanel();
		break;
	}

	UE_LOG(LogTemp, Log, TEXT("RightPanelController: Switched to panel %d with custom index %d"), static_cast<int32>(PanelType), CustomIndex);
}

void URightPanelController::TogglePanel(ERightPanelType PanelType)
{
	if (!RightPanel)
	{
		UE_LOG(LogTemp, Warning, TEXT("RightPanelController: RightPanel is null"));
		return;
	}

	// Properties 버튼 클릭 시 선택된 Actor 타입 확인
	if (PanelType == ERightPanelType::Properties)
	{
		// PlayerController에서 선택된 Actor 가져오기
		AThirdMotionPlayerController* PC = Cast<AThirdMotionPlayerController>(GetWorld()->GetFirstPlayerController());
		if (PC)
		{
			AActor* SelectedActor = PC->GetSelectedActor();
			if (SelectedActor)
			{
				// LightComponent로 Light Actor인지 확인 (일관성 있게)
				ULightComponent* LightComp = SelectedActor->FindComponentByClass<ULightComponent>();
				if (LightComp)
				{
					// Light Actor인 경우 토글 처리
					if (bIsPanelVisible && CurrentPanel == PanelType)
					{
						HidePanel();
					}
					else
					{
						// Properties 패널로 전환 (WidgetSwitcher)
						CurrentPanel = PanelType;
						bIsPanelVisible = true;
						RightPanel->SetRightPanelVisibility(true);
						RightPanel->SetWidgetSwitcherIndex(static_cast<int32>(PanelType));

						// MeshSettingsWidget 숨기고 LightWidget 표시
						if (RightPanel->MeshSettingsWidget)
						{
							RightPanel->MeshSettingsWidget->SetVisibility(ESlateVisibility::Collapsed);
						}

						if (RightPanel->LightWidget)
						{
							RightPanel->LightWidget->InitializeWithLightActor(SelectedActor);
							RightPanel->LightWidget->SetVisibility(ESlateVisibility::Visible);
						}
					}
					UE_LOG(LogTemp, Log, TEXT("RightPanelController: Light actor selected - showing LightWidget"));
					return;
				}
				else
				{
					// Mesh Actor인 경우: MeshSettingsWidget 표시, LightWidget 숨김
					if (RightPanel->LightWidget)
					{
						RightPanel->LightWidget->SetVisibility(ESlateVisibility::Collapsed);
					}
					if (RightPanel->MeshSettingsWidget)
					{
						RightPanel->MeshSettingsWidget->SetVisibility(ESlateVisibility::Visible);
					}
				}
			}
		}
	}

	// 같은 패널을 다시 클릭하면 숨김
	if (bIsPanelVisible && CurrentPanel == PanelType)
	{
		HidePanel();
	}
	else
	{
		// 다른 패널이거나 숨겨진 상태면 전환
		SwitchToPanel(PanelType);
	}
}

void URightPanelController::HidePanel()
{
	if (!RightPanel)
	{
		UE_LOG(LogTemp, Warning, TEXT("RightPanelController: RightPanel is null"));
		return;
	}

	bIsPanelVisible = false;
	RightPanel->SetRightPanelVisibility(false);

	UE_LOG(LogTemp, Log, TEXT("RightPanelController: Panel hidden"));
}

void URightPanelController::BindMaterialPanel(UMaterialGeneratePanel* InView)
{
	MeshWidgetController->BindMaterialPanel(InView);
}

void URightPanelController::InitializeScenePanel()
{
	// Scene 패널 초기화 로직
	UE_LOG(LogTemp, Log, TEXT("RightPanelController: Scene panel initialized"));
}

void URightPanelController::InitializePropertiesPanel()
{
	// Properties 패널 초기화 로직
	UE_LOG(LogTemp, Log, TEXT("RightPanelController: Properties panel initialized"));
}

void URightPanelController::InitializeUserListPanel()
{
	// UserList 패널 초기화 로직
	UE_LOG(LogTemp, Log, TEXT("RightPanelController: UserList panel initialized"));
}
