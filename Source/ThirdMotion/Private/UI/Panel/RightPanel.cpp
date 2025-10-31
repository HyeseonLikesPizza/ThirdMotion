#include "UI/Panel/RightPanel.h"
#include "UI/Widget/SceneItemWidget.h"
#include "UI/Widget/SceneListWidget.h"
#include "UI/Widget/LightWidget.h"
#include "UI/WidgetController/RightPanelController.h"
#include "UI/WidgetController/SceneController.h"
#include "Framework/ThirdMotionPlayerController.h"
#include "Components/TextBlock.h"
#include "Components/WidgetSwitcher.h"
#include "Components/LightComponent.h"
#include "Engine/World.h"
#include "Framework/Application/SlateApplication.h"
#include "Engine/GameInstance.h"


//if WITH_EDITOR: 에디터 전용 코드를 구분하는 전처리 지시자
#if WITH_EDITOR
// 에디터에서 실행 
#include "Engine/Selection.h"
#include "Editor.h"
#endif

/**
 * Row Widget 생성 - View만 담당
 */

void URightPanel::NativeConstruct()
{
	Super::NativeConstruct();

	// RightPanelController 초기화
	InitializeRightPanelController();

	// PlayerController의 OnActorSelected 델리게이트 바인딩
	if (AThirdMotionPlayerController* PC = Cast<AThirdMotionPlayerController>(GetOwningPlayer()))
	{
		PC->OnActorSelected.AddDynamic(this, &URightPanel::OnActorSelected);
		UE_LOG(LogTemp, Log, TEXT("RightPanel: OnActorSelected delegate bound"));
	}

	// LightWidget 초기 상태: 숨김
	if (LightWidget)
	{
		LightWidget->SetLightWidgetVisibility(false);
	}

	// PropertiesSwitcher 바인딩 확인
	if (PropertiesSwitcher)
	{
		UE_LOG(LogTemp, Log, TEXT("RightPanel: PropertiesSwitcher is bound successfully"));
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("RightPanel: PropertiesSwitcher is NOT bound - check Blueprint binding"));
	}

	UE_LOG(LogTemp, Log, TEXT("RightPanel: NativeConstruct completed"));
}

void URightPanel::NativeDestruct()
{
	// 델리게이트 언바인딩 (메모리 누수 방지)
	if (AThirdMotionPlayerController* PC = Cast<AThirdMotionPlayerController>(GetOwningPlayer()))
	{
		PC->OnActorSelected.RemoveDynamic(this, &URightPanel::OnActorSelected);
	}

	Super::NativeDestruct();
}

void URightPanel::InitializeRightPanelController()
{
	if (!RightPanelController)
	{
		RightPanelController = NewObject<URightPanelController>(this);
		RightPanelController->InitializeWithRightPanel(this);

		UE_LOG(LogTemp, Log, TEXT("RightPanel: RightPanelController initialized"));
	}
}


void URightPanel::SetWidgetSwitcherIndex(int32 Index)
{
	if (!WidgetSwitcher_Right)
	{
		UE_LOG(LogTemp, Warning, TEXT("RightPanel View: WidgetSwitcher_Right is null"));
		return;
	}

	// WidgetSwitcher 인덱스 변경 (View 기능만)
	WidgetSwitcher_Right->SetActiveWidgetIndex(Index);

	UE_LOG(LogTemp, Log, TEXT("RightPanel View: WidgetSwitcher set to index %d"), Index);
}

void URightPanel::SetRightPanelVisibility(bool bVisible)
{
	if (bVisible)
	{
		SetVisibility(ESlateVisibility::Visible);
		UE_LOG(LogTemp, Log, TEXT("RightPanel View: Panel shown"));
	}
	else
	{
		SetVisibility(ESlateVisibility::Collapsed);
		UE_LOG(LogTemp, Log, TEXT("RightPanel View: Panel hidden"));
	}
}

USceneController* URightPanel::GetSceneController() const
{
	if (SceneListWidget)
	{
		return SceneListWidget->GetSceneController();
	}
	return nullptr;
}

void URightPanel::OnActorSelected(AActor* SelectedActor)
{
	// Actor 선택 시 자동으로 패널을 표시하지 않음
	// Properties 버튼을 클릭해야 패널이 표시됨
	UE_LOG(LogTemp, Log, TEXT("RightPanel: Actor selected (use Properties button to view details)"));
}

void URightPanel::HandleLightActorSelection(AActor* LightActor)
{
	if (!LightWidget)
	{
		UE_LOG(LogTemp, Warning, TEXT("RightPanel: LightWidget is null"));
		return;
	}

	// LightWidget 초기화 및 표시
	LightWidget->InitializeWithLightActor(LightActor);
	LightWidget->SetLightWidgetVisibility(true);

	UE_LOG(LogTemp, Log, TEXT("RightPanel: Light Actor selected - LightWidget displayed"));
}
