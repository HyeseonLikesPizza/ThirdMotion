
#include "UI/Widget/MainWidget.h"

#include "Components/CanvasPanelSlot.h"
#include "Components/Widget.h"
#include "UI/Widget/ViewportWidget.h"
#include "UI/Panel/BottomBar.h"
#include "UI/Panel/RightPanel.h"
#include "Blueprint/WidgetTree.h"
#include "Camera/CameraActor.h"

void UMainWidget::NativeConstruct()
{
	Super::NativeConstruct();

	// Setup 3D viewport
	//SetupViewport();

	// BottomBar와 RightPanel 연결
	if (BottomBar && RightPanel)
	{
		UBottomBar* BottomBarWidget = Cast<UBottomBar>(BottomBar);
		URightPanel* RightPanelWidget = Cast<URightPanel>(RightPanel);

		if (BottomBarWidget && RightPanelWidget)
		{
			BottomBarWidget->InitializeWithRightPanel(RightPanelWidget);
			UE_LOG(LogTemp, Log, TEXT("MainWidget: BottomBar and RightPanel connected"));
		}
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("MainWidget: Failed to cast BottomBar or RightPanel"));
		}
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("MainWidget: BottomBar or RightPanel is null"));
	}
}

void UMainWidget::NativeDestruct()
{
	Super::NativeDestruct();
}

/*
void UMainWidget::SetupViewport()
{
	if (!ViewportContainer || !ViewportWidgetClass)
	{
		return;
	}

	// ViewportWidget 생성
	ViewportWidget = CreateWidget<UViewportWidget>(GetWorld(), ViewportWidgetClass);
	if (ViewportWidget)
	{
		// ViewportContainer에 ViewportWidget 추가
		ViewportContainer->ClearChildren();
		ViewportContainer->AddChild(ViewportWidget);

		// 카메라 액터 생성
		if (UWorld* World = GetWorld())
		{
			// 뷰포트용 카메라 생성
			ACameraActor* ViewportCamera = World->SpawnActor<ACameraActor>();
			ViewportCamera->SetActorLocation(FVector(-500, 0, 200));
			ViewportCamera->SetActorRotation(FRotator(-20, 0, 0));

			// Player Controller의 뷰 타겟 설정
			if (APlayerController* PC = World->GetFirstPlayerController())
			{
				PC->SetViewTarget(ViewportCamera);
			}
		}
	}
}*/
