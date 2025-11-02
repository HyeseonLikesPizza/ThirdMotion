// Fill out your copyright notice in the Description page of Project Settings.

#include "UI/WidgetController/RightPanelController.h"
#include "ThirdMotion/ThirdMotion.h"
#include "UI/Panel/RightPanel.h"
#include "UI/Widget/LightWidget.h"
#include "UI/Widget/Mesh/MeshSettingsWidget.h"
#include "UI/WidgetController/MeshWidgetController.h"
#include "Edit/AssetResolver.h"
#include "Edit/EditSyncComponent.h"
#include "Edit/EditTypes.h"
#include "Framework/ThirdMotionPlayerController.h"
#include "Components/LightComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Components/WidgetSwitcher.h"
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
	UE_LOG(LogTemp, Warning, TEXT("RightPanelController::TogglePanel called with PanelType: %d"), static_cast<int32>(PanelType));

	if (!RightPanel)
	{
		UE_LOG(LogTemp, Warning, TEXT("RightPanelController: RightPanel is null"));
		return;
	}

	// Properties 버튼 클릭 시 선택된 Actor 타입 확인
	if (PanelType == ERightPanelType::Properties)
	{
		// 토글 처리
		if (bIsPanelVisible && CurrentPanel == PanelType)
		{
			HidePanel();
			return;
		}
		else
		{
			// Properties 패널로 전환
			CurrentPanel = PanelType;
			bIsPanelVisible = true;
			RightPanel->SetRightPanelVisibility(true);
			RightPanel->SetWidgetSwitcherIndex(static_cast<int32>(PanelType));

			// PlayerController에서 선택된 Actor 가져오기
			AThirdMotionPlayerController* PC = Cast<AThirdMotionPlayerController>(GetWorld()->GetFirstPlayerController());
			AActor* SelectedActor = PC ? PC->GetSelectedActor() : nullptr;

			// PresetType에 따라 PropertiesSwitcher 인덱스 설정
			UpdatePropertiesSwitcherByActor(SelectedActor);
			return;
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
	if (!MeshWidgetController)
	{
		UE_LOG(LogTemp, Warning, TEXT("RightPanelController: MeshWidgetController is null"));
		return;
	}

	MeshWidgetController->BindMaterialPanel(InView);
}

void URightPanelController::InitializeScenePanel()
{
	// Scene 패널 초기화 로직
	UE_LOG(LogTemp, Log, TEXT("RightPanelController: Scene panel initialized"));
}

void URightPanelController::InitializePropertiesPanel()
{
	UE_LOG(LogTemp, Warning, TEXT("RightPanelController: InitializePropertiesPanel START"));

	// Properties 패널 초기화 로직
	// 선택된 Actor 확인하고 PropertiesSwitcher 인덱스 설정
	AThirdMotionPlayerController* PC = Cast<AThirdMotionPlayerController>(GetWorld()->GetFirstPlayerController());
	if (PC)
	{
		AActor* SelectedActor = PC->GetSelectedActor();
		UE_LOG(LogTemp, Warning, TEXT("RightPanelController: SelectedActor = %s"), SelectedActor ? *SelectedActor->GetName() : TEXT("NULL"));
		UpdatePropertiesSwitcherByActor(SelectedActor);
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("RightPanelController: PlayerController is NULL"));
		// Actor 선택 없으면 기본값 0 (Mesh)
		if (RightPanel && RightPanel->PropertiesSwitcher)
		{
			RightPanel->PropertiesSwitcher->SetActiveWidgetIndex(0);
			UE_LOG(LogTemp, Log, TEXT("RightPanelController: No actor selected - PropertiesSwitcher set to default index 0"));
		}
	}

	UE_LOG(LogTemp, Log, TEXT("RightPanelController: Properties panel initialized"));
}

void URightPanelController::UpdatePropertiesSwitcherByActor(AActor* SelectedActor)
{
	UE_LOG(LogTemp, Warning, TEXT("RightPanelController: UpdatePropertiesSwitcherByActor START - Actor: %s"), SelectedActor ? *SelectedActor->GetName() : TEXT("NULL"));

	if (!RightPanel)
	{
		UE_LOG(LogTemp, Warning, TEXT("RightPanelController: RightPanel is null"));
		return;
	}

	// 기본값: index 0 (Mesh)
	int32 PropertiesIndex = 0;
	bool bTagFound = false;

	if (SelectedActor)
	{
		// 1. StaticMeshComponent의 ComponentTags 확인
		TArray<UStaticMeshComponent*> StaticMeshComponents;
		SelectedActor->GetComponents<UStaticMeshComponent>(StaticMeshComponents);

		if (StaticMeshComponents.Num() > 0)
		{
			UE_LOG(LogTemp, Log, TEXT("RightPanelController: Actor has %d StaticMeshComponents"), StaticMeshComponents.Num());

			for (UStaticMeshComponent* MeshComp : StaticMeshComponents)
			{
				if (!MeshComp) continue;

				TArray<FName> ComponentTags = MeshComp->ComponentTags;
				UE_LOG(LogTemp, Log, TEXT("RightPanelController: StaticMeshComponent '%s' has %d ComponentTags"), *MeshComp->GetName(), ComponentTags.Num());

				for (const FName& Tag : ComponentTags)
				{
					FString TagString = Tag.ToString();
					UE_LOG(LogTemp, Log, TEXT("RightPanelController: Checking ComponentTag: %s"), *TagString);

					// ComponentTag가 "Light"이면 PropertiesSwitcher index 2
					if (TagString.Equals(TEXT("Light"), ESearchCase::IgnoreCase))
					{
						PropertiesIndex = 2;
						bTagFound = true;
						UE_LOG(LogTemp, Log, TEXT("RightPanelController: StaticMeshComponent tag 'Light' found - PropertiesSwitcher index 2"));
						break;
					}
				}

				if (bTagFound) break;
			}
		}

		// 2. ComponentTag로 찾지 못했으면 Actor Tag 확인
		if (!bTagFound)
		{
			TArray<FName> ActorTags = SelectedActor->Tags;
			if (ActorTags.Num() > 0)
			{
				UE_LOG(LogTemp, Log, TEXT("RightPanelController: Actor has %d tags"), ActorTags.Num());

				for (const FName& Tag : ActorTags)
				{
					FString TagString = Tag.ToString();
					UE_LOG(LogTemp, Log, TEXT("RightPanelController: Checking tag: %s"), *TagString);

					// Tag 이름에 따라 PropertiesSwitcher 인덱스 설정
					if (TagString.Contains(TEXT("Mesh")) || TagString.Contains(TEXT("mesh")))
					{
						PropertiesIndex = 0;
						bTagFound = true;
						UE_LOG(LogTemp, Log, TEXT("RightPanelController: Tag 'Mesh' found - PropertiesSwitcher index 0"));
						break;
					}
					else if (TagString.Contains(TEXT("Light")) || TagString.Contains(TEXT("light")))
					{
						PropertiesIndex = 1;
						bTagFound = true;
						// LightWidget 초기화
						if (RightPanel->LightWidget)
						{
							RightPanel->LightWidget->InitializeWithLightActor(SelectedActor);
						}
						UE_LOG(LogTemp, Log, TEXT("RightPanelController: Tag 'Light' found - PropertiesSwitcher index 1"));
						break;
					}
					else if (TagString.Contains(TEXT("Camera")) || TagString.Contains(TEXT("camera")))
					{
						PropertiesIndex = 2;
						bTagFound = true;
						UE_LOG(LogTemp, Log, TEXT("RightPanelController: Tag 'Camera' found - PropertiesSwitcher index 2"));
						break;
					}
				}
			}
		}

		// 2. Tag가 없거나 매칭되지 않으면 PresetTag로 폴백
		if (!bTagFound)
		{
			UE_LOG(LogTemp, Log, TEXT("RightPanelController: No matching tag found, falling back to PresetTag"));

			UEditSyncComponent* EditComp = SelectedActor->FindComponentByClass<UEditSyncComponent>();
			if (EditComp)
			{
				const FEditMeta& Meta = EditComp->GetMeta();
				const FGameplayTag& PresetTag = Meta.PresetTag;

				// AssetResolver를 통해 LibraryRow 가져오기
				UAssetResolver* Resolver = GetWorld()->GetSubsystem<UAssetResolver>();
				if (Resolver && PresetTag.IsValid())
				{
					const FLibraryRow* Row = Resolver->FindRowByTag(PresetTag);
					if (Row)
					{
						// PresetType에 따라 PropertiesSwitcher 인덱스 설정
						switch (Row->PresetType)
						{
						case EPresetType::Mesh:
							PropertiesIndex = 0;
							UE_LOG(LogTemp, Log, TEXT("RightPanelController: Mesh actor (via PresetTag) - PropertiesSwitcher index 0"));
							break;
						case EPresetType::Light:
							PropertiesIndex = 1;
							// LightWidget 초기화
							if (RightPanel->LightWidget)
							{
								RightPanel->LightWidget->InitializeWithLightActor(SelectedActor);
							}
							UE_LOG(LogTemp, Log, TEXT("RightPanelController: Light actor (via PresetTag) - PropertiesSwitcher index 1"));
							break;
						case EPresetType::Camera:
							PropertiesIndex = 2;
							UE_LOG(LogTemp, Log, TEXT("RightPanelController: Camera actor (via PresetTag) - PropertiesSwitcher index 2"));
							break;
						default:
							PropertiesIndex = 0;
							UE_LOG(LogTemp, Warning, TEXT("RightPanelController: Unknown PresetType - defaulting to index 0"));
							break;
						}
					}
					else
					{
						UE_LOG(LogTemp, Warning, TEXT("RightPanelController: LibraryRow not found for PresetTag: %s - using default index 0"), *PresetTag.ToString());
					}
				}
				else
				{
					UE_LOG(LogTemp, Warning, TEXT("RightPanelController: AssetResolver is null or PresetTag is invalid - using default index 0"));
				}
			}
			else
			{
				UE_LOG(LogTemp, Warning, TEXT("RightPanelController: EditSyncComponent not found - using default index 0"));
			}
		}
	}
	else
	{
		UE_LOG(LogTemp, Log, TEXT("RightPanelController: No actor selected - using default index 0"));
	}

	// PropertiesSwitcher 인덱스 설정
	if (RightPanel->PropertiesSwitcher)
	{
		RightPanel->PropertiesSwitcher->SetActiveWidgetIndex(PropertiesIndex);
		UE_LOG(LogTemp, Log, TEXT("RightPanelController: PropertiesSwitcher set to index %d"), PropertiesIndex);
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("RightPanelController: PropertiesSwitcher is null"));
	}
}

void URightPanelController::InitializeUserListPanel()
{
	// UserList 패널 초기화 로직
	UE_LOG(LogTemp, Log, TEXT("RightPanelController: UserList panel initialized"));
}
