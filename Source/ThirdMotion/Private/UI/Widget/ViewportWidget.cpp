#include "UI/Widget/ViewportWidget.h"
#include "Engine/Engine.h"
#include "Engine/GameViewportClient.h"
#include "Slate/SceneViewport.h"
#include "Widgets/SViewport.h"
#include "Widgets/SOverlay.h"
#include "Engine/LocalPlayer.h"
#include "Framework/Application/SlateApplication.h"

TSharedRef<SWidget> UViewportWidget::RebuildWidget()
{
	// SViewport 생성
	ViewportWidget = SNew(SViewport)
		.EnableGammaCorrection(false)
		.IsEnabled(true)
		.ShowEffectWhenDisabled(false);
    
	// Overlay로 래핑 (UWidget이 요구하는 형식)
	RootWidget = SNew(SOverlay)
		+ SOverlay::Slot()
		.HAlign(HAlign_Fill)
		.VAlign(VAlign_Fill)
		[
			ViewportWidget.ToSharedRef()
		];
    
	// 기존 GameViewportClient 사용
	if (UGameViewportClient* GameViewportClient = GetWorld()->GetGameViewport())
	{
		// SceneViewport 생성
		SceneViewport = MakeShareable(new FSceneViewport(
			GameViewportClient,
			ViewportWidget
		));
        
		ViewportWidget->SetViewportInterface(SceneViewport.ToSharedRef());
	}
    
	return RootWidget.ToSharedRef();
}

void UViewportWidget::ReleaseSlateResources(bool bReleaseChildren)
{
	Super::ReleaseSlateResources(bReleaseChildren);
    
	RootWidget.Reset();
	ViewportWidget.Reset();
	SceneViewport.Reset();
}