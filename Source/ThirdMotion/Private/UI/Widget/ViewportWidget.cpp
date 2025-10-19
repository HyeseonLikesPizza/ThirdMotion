#include "UI/Widget/ViewportWidget.h"
#include "Engine/Engine.h"
#include "Engine/GameViewportClient.h"
#include "Slate/SceneViewport.h"
#include "Widgets/SViewport.h"
#include "Widgets/SOverlay.h"
#include "Engine/LocalPlayer.h"
#include "Framework/Application/SlateApplication.h"
#include "Components/Slider.h"
#include "Engine/DirectionalLight.h"
#include "Components/DirectionalLightComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Framework/ThirdMotionPlayerController.h"

/*
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

			// 마우스 커서가 보이도록 설정
			SceneViewport->ShowCursor(true);
		}
	}

	return RootWidget.ToSharedRef();
}*/

void UViewportWidget::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);

	// SceneViewport가 매 프레임 업데이트되도록 강제
	/*if (SceneViewport.IsValid())
	{
		// 뷰포트 크기 업데이트
		FVector2D Size = MyGeometry.GetLocalSize();
		if (Size.X > 0 && Size.Y > 0)
		{
			SceneViewport->UpdateViewportRHI(false, Size.X, Size.Y, EWindowMode::Windowed, PF_Unknown);
		}

		// 렌더링 강제 업데이트
		SceneViewport->InvalidateDisplay();
	}*/
}

void UViewportWidget::ReleaseSlateResources(bool bReleaseChildren)
{
	Super::ReleaseSlateResources(bReleaseChildren);

	RootWidget.Reset();
	ViewportWidget.Reset();
	SceneViewport.Reset();
}

void UViewportWidget::NativeConstruct()
{
	Super::NativeConstruct();

	// DirectionalLight 찾기
	TArray<AActor*> FoundLights;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), ADirectionalLight::StaticClass(), FoundLights);

	if (FoundLights.Num() > 0)
	{
		DirectionalLight = Cast<ADirectionalLight>(FoundLights[0]);
	}

	// Slider_Light 콜백 바인딩
	if (Slider_Light)
	{
		Slider_Light->OnValueChanged.AddDynamic(this, &UViewportWidget::OnLightSliderValueChanged);

		// 초기값 설정 (0.0 ~ 1.0을 -90 ~ 90도로 매핑)
		if (DirectionalLight)
		{
			float CurrentPitch = DirectionalLight->GetActorRotation().Pitch;
			// -90 ~ 90을 0.0 ~ 1.0으로 변환
			float NormalizedValue = (CurrentPitch + 90.0f) / 180.0f;
			Slider_Light->SetValue(NormalizedValue);
		}
		else
		{
			Slider_Light->SetValue(0.5f); // 기본값 (0도)
		}
	}
}

void UViewportWidget::OnLightSliderValueChanged(float Value)
{
	if (!DirectionalLight)
	{
		return;
	}

	// Slider 값 (0.0 ~ 1.0)을 각도 (-90 ~ 90)로 변환
	float Pitch = FMath::Lerp(-90.0f, 90.0f, Value);

	FRotator NewRotation = DirectionalLight->GetActorRotation();
	NewRotation.Pitch = Pitch;

	// 로컬에서 즉시 적용
	DirectionalLight->SetActorRotation(NewRotation);

	// 네트워크 동기화 - PlayerController를 통해 서버에 전송
	if (AThirdMotionPlayerController* PC = Cast<AThirdMotionPlayerController>(GetOwningPlayer()))
	{
		PC->Server_UpdateDirectionalLightRotation(NewRotation);
	}
}