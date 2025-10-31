
#include "UI/Widget/MainWidget.h"

#include "Components/CanvasPanelSlot.h"
#include "Components/Widget.h"
#include "UI/Widget/ViewportWidget.h"
#include "UI/Panel/BottomBar.h"
#include "UI/Panel/RightPanel.h"
#include "UI/Panel/LibraryPanel.h"
#include "UI/Panel/BottomView.h"
#include "Blueprint/WidgetTree.h"
#include "Camera/CameraActor.h"

void UMainWidget::NativeConstruct()
{
	Super::NativeConstruct();

	// ViewportWidget 찾기 (Blueprint에서 BindWidget으로 연결 안된 경우 대비)
	if (!ViewportWidget && WidgetTree)
	{
		// 여러 가능한 이름으로 시도
		TArray<FName> PossibleNames = {
			FName(TEXT("ViewportWidget")),
			FName(TEXT("WBP_Viewport")),
			FName(TEXT("Viewport")),
			FName(TEXT("ViewportWidget_0"))
		};

		for (const FName& Name : PossibleNames)
		{
			ViewportWidget = Cast<UViewportWidget>(WidgetTree->FindWidget(Name));
			if (ViewportWidget)
			{
				UE_LOG(LogTemp, Log, TEXT("MainWidget: ViewportWidget found via WidgetTree with name: %s"), *Name.ToString());
				break;
			}
		}

		if (!ViewportWidget)
		{
			UE_LOG(LogTemp, Warning, TEXT("MainWidget: ViewportWidget not found! Tried all possible names."));
		}
	}

	// BottomBar와 RightPanel 연결
	if (BottomBar && RightPanel)
	{
		UBottomBar* BottomBarWidget = Cast<UBottomBar>(BottomBar);
		URightPanel* RightPanelWidget = Cast<URightPanel>(RightPanel);

		if (BottomBarWidget && RightPanelWidget)
		{
			BottomBarWidget->InitializeWithRightPanel(RightPanelWidget);
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

	// BottomBar와 LibraryPanel 연결
	if (BottomBar && LibraryPanel)
	{
		UBottomBar* BottomBarWidget = Cast<UBottomBar>(BottomBar);
		ULibraryPanel* LibraryPanelWidget = Cast<ULibraryPanel>(LibraryPanel);

		if (BottomBarWidget && LibraryPanelWidget)
		{
			BottomBarWidget->InitializeWithLibraryPanel(LibraryPanelWidget);
		}
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("MainWidget: Failed to cast BottomBar or LibraryPanel"));
		}
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("MainWidget: BottomBar or LibraryPanel is null"));
	}

	// BottomBar와 BottomView 연결
	if (BottomBar && BottomView)
	{
		UBottomBar* BottomBarWidget = Cast<UBottomBar>(BottomBar);
		UBottomView* BottomViewWidget = Cast<UBottomView>(BottomView);

		if (BottomBarWidget && BottomViewWidget)
		{
			BottomBarWidget->InitializeWithBottomView(BottomViewWidget);

			// BottomView에 RightPanel 참조 전달 (view toggle 분기 처리)
			URightPanel* RightPanelWidget = Cast<URightPanel>(RightPanel);
			if (RightPanelWidget)
			{
				BottomViewWidget->InitializeWithRightPanel(RightPanelWidget);
			}
		}
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("MainWidget: Failed to cast BottomBar or BottomView"));
		}
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("MainWidget: BottomBar or BottomView is null"));
	}

}

void UMainWidget::NativeDestruct()
{
	Super::NativeDestruct();
}

