
#include "UI/Widget/MainWidget.h"

#include "Components/CanvasPanelSlot.h"
#include "Components/Widget.h"
#include "UI/Widget/ViewportWidget.h"
#include "UI/Panel/BottomBar.h"
#include "UI/Panel/RightPanel.h"
#include "UI/Panel/LibraryPanel.h"
#include "Blueprint/WidgetTree.h"
#include "Camera/CameraActor.h"

void UMainWidget::NativeConstruct()
{
	Super::NativeConstruct();

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
	
}

void UMainWidget::NativeDestruct()
{
	Super::NativeDestruct();
}

