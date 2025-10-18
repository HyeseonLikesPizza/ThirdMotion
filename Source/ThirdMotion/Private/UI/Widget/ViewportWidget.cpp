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
	// SViewport 생성 - 이 뷰포트에서 3D World를 렌더링
	ViewportWidget = SNew(SViewport)
		.EnableGammaCorrection(false)
		.IsEnabled(true)
		.ShowEffectWhenDisabled(false)
		.EnableBlending(true);

	// Overlay로 래핑 (UWidget이 요구하는 형식)
	RootWidget = SNew(SOverlay)
		+ SOverlay::Slot()
		.HAlign(HAlign_Fill)
		.VAlign(VAlign_Fill)
		[
			ViewportWidget.ToSharedRef()
		];

	// 기존 GameViewportClient를 재사용하여 3D 렌더링 공유
	if (UWorld* World = GetWorld())
	{
		if (UGameViewportClient* GameViewportClient = World->GetGameViewport())
		{
			// SceneViewport 생성 - GameViewportClient를 공유하여 같은 World 렌더링
			SceneViewport = MakeShareable(new FSceneViewport(
				GameViewportClient,
				ViewportWidget
			));

			ViewportWidget->SetViewportInterface(SceneViewport.ToSharedRef());
		}
	}

	return RootWidget.ToSharedRef();
}

void UViewportWidget::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);

	// SceneViewport가 매 프레임 업데이트되도록 강제
	if (SceneViewport.IsValid())
	{
		// 뷰포트 크기 업데이트
		FVector2D Size = MyGeometry.GetLocalSize();
		if (Size.X > 0 && Size.Y > 0)
		{
			SceneViewport->UpdateViewportRHI(false, Size.X, Size.Y, EWindowMode::Windowed, PF_Unknown);
		}

		// 렌더링 강제 업데이트
		SceneViewport->InvalidateDisplay();
	}
}

void UViewportWidget::ReleaseSlateResources(bool bReleaseChildren)
{
	Super::ReleaseSlateResources(bReleaseChildren);

	RootWidget.Reset();
	ViewportWidget.Reset();
	SceneViewport.Reset();
}