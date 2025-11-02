
#include "Edit/HighlightComponent.h"

#include "ThirdMotion/ThirdMotion.h"
#include "Edit/EditSyncComponent.h"
#include "Edit/AssetResolver.h"
#include "Edit/EditTypes.h"
#include "Framework/ThirdMotionPlayerController.h"
#include "UI/Widget/MainWidget.h"
#include "UI/Panel/RightPanel.h"
#include "UI/WidgetController/RightPanelController.h"
#include "Components/WidgetSwitcher.h"
#include "GameplayTagContainer.h"
#include "Blueprint/UserWidget.h"
#include "Blueprint/WidgetBlueprintLibrary.h"


UHighlightComponent::UHighlightComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
}

void UHighlightComponent::EnableHighlight(bool bEnable)
{
	AActor* Owner = GetOwner();
	if (!Owner) return;

	TArray<UPrimitiveComponent*> Prims;
	Owner->GetComponents<UPrimitiveComponent>(Prims, false);

	for (UPrimitiveComponent* Prim : Prims)
	{
		if (!Prim) continue;

		Prim->SetRenderCustomDepth(bEnable);
		if (bEnable)
		{
			Prim->SetCustomDepthStencilValue(StencilValue);
		}
	}

	// Highlight 활성화 시 PresetType에 따라 PropertiesSwitcher 인덱스 설정
	if (bEnable)
	{
		// EditSyncComponent에서 PresetTag 가져오기
		UEditSyncComponent* EditComp = Owner->FindComponentByClass<UEditSyncComponent>();
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
					// PresetType에 따라 PropertiesSwitcher 인덱스 결정
					int32 PropertiesIndex = 0;

					switch (Row->PresetType)
					{
					case EPresetType::Mesh:
						PropertiesIndex = 1;
						UE_LOG(LogTemp, Log, TEXT("HighlightComponent: Mesh PresetType - PropertiesSwitcher index 1"));
						break;
					case EPresetType::Light:
						PropertiesIndex = 2;
						UE_LOG(LogTemp, Log, TEXT("HighlightComponent: Light PresetType - PropertiesSwitcher index 2"));
						break;
					case EPresetType::Tool:
						PropertiesIndex = 3;
						UE_LOG(LogTemp, Log, TEXT("HighlightComponent: Tool PresetType - PropertiesSwitcher index 3"));
						break;
					case EPresetType::Camera:
						PropertiesIndex = 2;
						UE_LOG(LogTemp, Log, TEXT("HighlightComponent: Camera PresetType - PropertiesSwitcher index 2"));
						break;
					default:
						PropertiesIndex = 0;
						UE_LOG(LogTemp, Warning, TEXT("HighlightComponent: Unknown PresetType - defaulting to index 0"));
						break;
					}

					// RightPanel 찾기 및 PropertiesSwitcher 설정
					AThirdMotionPlayerController* PC = Cast<AThirdMotionPlayerController>(GetWorld()->GetFirstPlayerController());
					if (PC && PC->GetPawn())
					{
						// UWidgetBlueprintLibrary를 통해 모든 MainWidget 검색
						TArray<UUserWidget*> FoundWidgets;
						UWidgetBlueprintLibrary::GetAllWidgetsOfClass(GetWorld(), FoundWidgets, UMainWidget::StaticClass(), false);

						for (UUserWidget* Widget : FoundWidgets)
						{
							UMainWidget* MainWidget = Cast<UMainWidget>(Widget);
							if (MainWidget && MainWidget->RightPanel)
							{
								URightPanel* RightPanel = Cast<URightPanel>(MainWidget->RightPanel);
								if (RightPanel)
								{
									// RightPanelController를 통해 Properties 패널로 전환
									URightPanelController* Controller = RightPanel->GetRightPanelController();
									if (Controller)
									{
										// Properties 패널로 전환 (RightPanel 자동 표시)
										Controller->SwitchToPanel(ERightPanelType::Properties);
										UE_LOG(LogTemp, Log, TEXT("HighlightComponent: Switched to Properties panel"));
									}

									// PropertiesSwitcher 인덱스 설정
									if (RightPanel->PropertiesSwitcher)
									{
										RightPanel->PropertiesSwitcher->SetActiveWidgetIndex(PropertiesIndex);
										UE_LOG(LogTemp, Log, TEXT("HighlightComponent: PropertiesSwitcher set to index %d"), PropertiesIndex);
									}
								}
								break;
							}
						}
					}
				}
				else
				{
					UE_LOG(LogTemp, Warning, TEXT("HighlightComponent: LibraryRow not found for PresetTag: %s"), *PresetTag.ToString());
				}
			}
		}
	}
}
