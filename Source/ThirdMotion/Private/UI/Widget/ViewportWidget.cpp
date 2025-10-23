#include "UI/Widget/ViewportWidget.h"
#include "Engine/Engine.h"
#include "Engine/GameViewportClient.h"
#include "Widgets/SOverlay.h"
#include "Engine/LocalPlayer.h"
#include "Framework/Application/SlateApplication.h"
#include "Components/Slider.h"
#include "Engine/DirectionalLight.h"
#include "Components/DirectionalLightComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Framework/ThirdMotionPlayerController.h"
#include "Net/UnrealNetwork.h"
#include "Components/Button.h"
#include "Components/TextBlock.h"
#include "HighResScreenshot.h"
#include "Misc/Paths.h"
#include "HAL/PlatformFileManager.h"

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

	// 네트워크 동기화는 OnRep_LightRotation에서 처리
	// NativeTick에서는 별도 처리 불필요

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

		// Mobility를 Movable로 설정 (런타임 회전 가능하도록)
		if (DirectionalLight)
		{
			if (UDirectionalLightComponent* LightComp = DirectionalLight->FindComponentByClass<UDirectionalLightComponent>())
			{
				LightComp->SetMobility(EComponentMobility::Movable);
			}

			// 초기 회전값 저장 (NativeTick에서 변경 감지용)
			LastLightRotation = DirectionalLight->GetActorRotation();
		}
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

	// ==================== Screenshot & Video Recording 버튼 바인딩 ====================

	if (ShootButton)
	{
		ShootButton->OnClicked.AddDynamic(this, &UViewportWidget::OnShootButtonClicked);
		UE_LOG(LogTemp, Log, TEXT("ViewportWidget: ShootButton bound"));
	}

	if (VideoButton)
	{
		VideoButton->OnClicked.AddDynamic(this, &UViewportWidget::OnVideoButtonClicked);
		UE_LOG(LogTemp, Log, TEXT("ViewportWidget: VideoButton bound"));
	}

	// VideoButtonText 초기 설정
	if (VideoButtonText)
	{
		VideoButtonText->SetText(FText::FromString(TEXT("Start Recording")));
	}
}

// ViewportWidget은 UUserWidget이므로 자동 복제되지 않음
// Multicast에서 수동으로 ReplicatedLightRotation 설정하고 OnRep 호출


// NetActor 패턴: 클라이언트에서는 자동 호출, 서버(Listen Server)에서는 수동 호출
void UViewportWidget::OnRep_LightRotation()
{

	// 라이트 회전 적용 (이미 Multicast에서 적용되었지만 안전을 위해)
	if (DirectionalLight)
	{
		DirectionalLight->SetActorRotation(ReplicatedLightRotation);
	}

	// 슬라이더 업데이트 (가장 중요!)
	if (Slider_Light)
	{
		// 각도 (-90 ~ 90)를 Slider 값 (0.0 ~ 1.0)으로 변환
		float NormalizedValue = (ReplicatedLightRotation.Pitch + 90.0f) / 180.0f;

		// 무한 루프 방지: OnValueChanged 콜백을 일시적으로 해제
		Slider_Light->OnValueChanged.RemoveDynamic(this, &UViewportWidget::OnLightSliderValueChanged);
		Slider_Light->SetValue(NormalizedValue);
		Slider_Light->OnValueChanged.AddDynamic(this, &UViewportWidget::OnLightSliderValueChanged);

		// 마지막 회전값 업데이트
		LastLightRotation = ReplicatedLightRotation;

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


	// 서버에게 회전 업데이트 요청 (RPC)
	if (AThirdMotionPlayerController* PC = Cast<AThirdMotionPlayerController>(GetOwningPlayer()))
	{
		PC->Server_UpdateDirectionalLightRotation(NewRotation);
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("[ViewportWidget] PlayerController is null!"));
	}
}

// ==================== Screenshot & Video Recording Implementation ====================

void UViewportWidget::OnShootButtonClicked()
{
	UE_LOG(LogTemp, Log, TEXT("ViewportWidget: Shoot button clicked"));
	TakeScreenshot();
}

void UViewportWidget::OnVideoButtonClicked()
{
	UE_LOG(LogTemp, Log, TEXT("ViewportWidget: Video button clicked"));

	if (bIsRecording)
	{
		StopRecording();
	}
	else
	{
		StartRecording();
	}
}

void UViewportWidget::TakeScreenshot()
{
	if (!GetWorld())
	{
		UE_LOG(LogTemp, Error, TEXT("ViewportWidget: GetWorld() is null, cannot take screenshot"));
		return;
	}

	// 프로젝트의 Saved/Screenshots 폴더 경로 생성
	FString SavedDir = FPaths::ProjectSavedDir() / TEXT("Screenshots");

	// 디렉토리가 존재하지 않으면 생성
	IPlatformFile& PlatformFile = FPlatformFileManager::Get().GetPlatformFile();
	if (!PlatformFile.DirectoryExists(*SavedDir))
	{
		PlatformFile.CreateDirectory(*SavedDir);
	}

	// 파일 이름 생성 (타임스탬프 + 카운터)
	FString Timestamp = FDateTime::Now().ToString(TEXT("%Y%m%d_%H%M%S"));
	FString FileName = FString::Printf(TEXT("Screenshot_%s_%04d.png"), *Timestamp, ScreenshotCounter);
	FString FullPath = SavedDir / FileName;

	// 스크린샷 캡처 (콘솔 커맨드 사용)
	if (APlayerController* PC = GetOwningPlayer())
	{
		// HighResShot 콘솔 커맨드 실행 (UI 포함)
		FString Command = FString::Printf(TEXT("HighResShot 1920x1080 filename=\"%s\""), *FullPath);
		PC->ConsoleCommand(Command);

		ScreenshotCounter++;

		UE_LOG(LogTemp, Log, TEXT("ViewportWidget: Screenshot taken - %s"), *FullPath);

		// 화면에 메시지 출력 (옵션)
		if (GEngine)
		{
			GEngine->AddOnScreenDebugMessage(-1, 3.0f, FColor::Green,
				FString::Printf(TEXT("Screenshot saved: %s"), *FileName));
		}
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("ViewportWidget: PlayerController is null, cannot take screenshot"));
	}
}

void UViewportWidget::StartRecording()
{
	if (bIsRecording)
	{
		UE_LOG(LogTemp, Warning, TEXT("ViewportWidget: Already recording"));
		return;
	}

	if (!GetWorld())
	{
		UE_LOG(LogTemp, Error, TEXT("ViewportWidget: GetWorld() is null, cannot start recording"));
		return;
	}

	APlayerController* PC = GetOwningPlayer();
	if (!PC)
	{
		UE_LOG(LogTemp, Error, TEXT("ViewportWidget: PlayerController is null, cannot start recording"));
		return;
	}

	// 녹화 시작 (Unreal의 기본 Sequence Recorder 사용)
	// 또는 StartMovieCapture 커맨드 사용
	PC->ConsoleCommand(TEXT("StartMovieCapture"));

	bIsRecording = true;

	// 버튼 텍스트 업데이트
	if (VideoButtonText)
	{
		VideoButtonText->SetText(FText::FromString(TEXT("Stop Recording")));
	}

	UE_LOG(LogTemp, Log, TEXT("ViewportWidget: Video recording started"));

	// 화면에 메시지 출력
	if (GEngine)
	{
		GEngine->AddOnScreenDebugMessage(-1, 3.0f, FColor::Red, TEXT("Recording started"));
	}
}

void UViewportWidget::StopRecording()
{
	if (!bIsRecording)
	{
		UE_LOG(LogTemp, Warning, TEXT("ViewportWidget: Not currently recording"));
		return;
	}

	if (!GetWorld())
	{
		UE_LOG(LogTemp, Error, TEXT("ViewportWidget: GetWorld() is null, cannot stop recording"));
		return;
	}

	APlayerController* PC = GetOwningPlayer();
	if (!PC)
	{
		UE_LOG(LogTemp, Error, TEXT("ViewportWidget: PlayerController is null, cannot stop recording"));
		return;
	}

	// 녹화 중지
	PC->ConsoleCommand(TEXT("StopMovieCapture"));

	bIsRecording = false;

	// 버튼 텍스트 업데이트
	if (VideoButtonText)
	{
		VideoButtonText->SetText(FText::FromString(TEXT("Start Recording")));
	}

	UE_LOG(LogTemp, Log, TEXT("ViewportWidget: Video recording stopped"));

	// 화면에 메시지 출력
	if (GEngine)
	{
		GEngine->AddOnScreenDebugMessage(-1, 3.0f, FColor::Green, TEXT("Recording stopped - Video saved to Saved/VideoCaptures"));
	}
}
