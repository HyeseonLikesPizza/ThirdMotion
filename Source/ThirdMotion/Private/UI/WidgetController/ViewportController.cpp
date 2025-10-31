#include "UI/WidgetController/ViewportController.h"
#include "Framework/ThirdMotionPlayerController.h"
#include "Engine/Engine.h"
#include "GameFramework/PlayerController.h"
#include "Kismet/GameplayStatics.h"
#include "HighResScreenshot.h"
#include "Misc/Paths.h"
#include "HAL/PlatformFileManager.h"
#include "HAL/PlatformProcess.h"
#include "Engine/DirectionalLight.h"
#include "Components/DirectionalLightComponent.h"

void UViewportController::Init()
{
	Super::Init();

	// 초기 상태 설정
	CurrentPanelType = EViewportPanelType::Light; // 기본값: Light 패널
	bIsRecording = false;
	ScreenshotCounter = 0;
	bIsViewportBoxVisible = false;

	// DirectionalLight 초기화
	InitializeDirectionalLight();

	UE_LOG(LogTemp, Log, TEXT("ViewportController: Initialized"));
}

void UViewportController::SetCameraView(ECameraView ViewType)
{
	AThirdMotionPlayerController* PC = Cast<AThirdMotionPlayerController>(GetPlayerController());
	if (PC)
	{
		PC->SetCameraView(ViewType);
	}
}

// ==================== DirectionalLight Control ====================

void UViewportController::InitializeDirectionalLight()
{
	UWorld* World = GetWorld();
	if (!World)
	{
		UE_LOG(LogTemp, Warning, TEXT("ViewportController: World is null, cannot initialize DirectionalLight"));
		return;
	}

	// DirectionalLight 찾기
	TArray<AActor*> FoundLights;
	UGameplayStatics::GetAllActorsOfClass(World, ADirectionalLight::StaticClass(), FoundLights);
	if (FoundLights.Num() > 0)
	{
		DirectionalLight = Cast<ADirectionalLight>(FoundLights[0]);
		if (DirectionalLight)
		{
			// 초기 회전값 저장
			LastLightRotation = DirectionalLight->GetActorRotation();

			// Slider 초기값 계산 (Pitch: -90 ~ 90 → Slider: 0 ~ 1)
			float NormalizedValue = (LastLightRotation.Pitch + 90.0f) / 180.0f;

			// Observer Pattern: View에게 초기값 알림
			OnLightRotationUpdated.Broadcast(NormalizedValue);

			UE_LOG(LogTemp, Log, TEXT("ViewportController: DirectionalLight found - Initial Pitch=%f"), LastLightRotation.Pitch);
		}
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("ViewportController: DirectionalLight not found in scene"));
	}
}

void UViewportController::OnLightSliderValueChanged(float Value)
{
	if (!DirectionalLight)
	{
		UE_LOG(LogTemp, Warning, TEXT("ViewportController: DirectionalLight is null"));
		return;
	}

	// Slider 값 (0~1)을 Pitch 각도 (-90~90)로 변환
	float NewPitch = (Value * 180.0f) - 90.0f;

	// 새 Rotation 생성 (Roll, Yaw는 유지)
	FRotator NewRotation = DirectionalLight->GetActorRotation();
	NewRotation.Pitch = NewPitch;

	// 로컬 미리보기 (즉시 반영)
	DirectionalLight->SetActorRotation(NewRotation);

	// PlayerController를 통해 서버 RPC 호출
	AThirdMotionPlayerController* PC = Cast<AThirdMotionPlayerController>(GetPlayerController());
	if (PC)
	{
		PC->Server_UpdateDirectionalLightRotation(NewRotation);
		UE_LOG(LogTemp, Log, TEXT("ViewportController: Slider changed - NewPitch=%f, Sending RPC"), NewPitch);
	}
}

// ==================== ViewportBox & Eye Button ====================

void UViewportController::ToggleViewportBox()
{
	// 상태 토글
	bIsViewportBoxVisible = !bIsViewportBoxVisible;

	// Observer Pattern: View에게 가시성 변경 알림
	OnViewportBoxVisibilityChanged.Broadcast(bIsViewportBoxVisible);

	UE_LOG(LogTemp, Log, TEXT("ViewportController: ViewportBox toggled - now %s"),
		bIsViewportBoxVisible ? TEXT("Visible") : TEXT("Hidden"));
}

// ==================== Panel Switching ====================

void UViewportController::SwitchToLightPanel()
{
	SwitchPanel(EViewportPanelType::Light);
}

void UViewportController::SwitchToScreenPanel()
{
	SwitchPanel(EViewportPanelType::Screen);
}

void UViewportController::SwitchToCubicPanel()
{
	SwitchPanel(EViewportPanelType::Cubic);
}

void UViewportController::SwitchPanel(EViewportPanelType NewPanelType)
{
	if (CurrentPanelType == NewPanelType)
	{
		UE_LOG(LogTemp, Log, TEXT("ViewportController: Already on panel %d"), static_cast<int32>(NewPanelType));
		return;
	}

	CurrentPanelType = NewPanelType;

	// Observer Pattern: View에게 패널 전환 알림
	OnPanelChanged.Broadcast(CurrentPanelType);

	UE_LOG(LogTemp, Log, TEXT("ViewportController: Switched to panel %d"), static_cast<int32>(CurrentPanelType));
}

// ==================== Screenshot & Video Recording ====================

void UViewportController::TakeScreenshot()
{
	APlayerController* PC = GetPlayerController();
	if (!PC)
	{
		UE_LOG(LogTemp, Error, TEXT("ViewportController: PlayerController is null, cannot take screenshot"));
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
	FString Command = FString::Printf(TEXT("HighResShot 1920x1080 filename=\"%s\""), *FullPath);
	PC->ConsoleCommand(Command);

	ScreenshotCounter++;

	UE_LOG(LogTemp, Log, TEXT("ViewportController: Screenshot taken - %s"), *FullPath);

	// Observer Pattern: View에게 스크린샷 완료 알림
	OnScreenshotTaken.Broadcast(FullPath);

	// 화면에 메시지 출력
	if (GEngine)
	{
		GEngine->AddOnScreenDebugMessage(-1, 3.0f, FColor::Green,
			FString::Printf(TEXT("Screenshot saved: %s"), *FileName));
	}

	// CreateProc를 사용하여 explorer.exe로 폴더 열기
	FString ProjectDir = FPaths::ProjectDir();
	FString ScreenshotPath = TEXT("Saved\\Screenshots");
	FString FullScreenshotPath = FPaths::ConvertRelativePathToFull(ProjectDir / ScreenshotPath);

	// Windows Explorer로 폴더 열기
	FPlatformProcess::CreateProc(TEXT("explorer.exe"), *FullScreenshotPath, true, false, false, nullptr, 0, nullptr, nullptr);
	UE_LOG(LogTemp, Log, TEXT("ViewportController: Opened folder via CreateProc - %s"), *FullScreenshotPath);
}

void UViewportController::ToggleRecording()
{
	if (bIsRecording)
	{
		StopRecording();
	}
	else
	{
		StartRecording();
	}
}

void UViewportController::StartRecording()
{
	if (bIsRecording)
	{
		UE_LOG(LogTemp, Warning, TEXT("ViewportController: Already recording"));
		return;
	}

	APlayerController* PC = GetPlayerController();
	if (!PC)
	{
		UE_LOG(LogTemp, Error, TEXT("ViewportController: PlayerController is null, cannot start recording"));
		return;
	}

	// 녹화 시작
	PC->ConsoleCommand(TEXT("StartMovieCapture"));

	bIsRecording = true;

	UE_LOG(LogTemp, Log, TEXT("ViewportController: Video recording started"));

	// Observer Pattern: View에게 녹화 상태 변경 알림
	OnRecordingStateChanged.Broadcast(bIsRecording);

	// 화면에 메시지 출력
	if (GEngine)
	{
		GEngine->AddOnScreenDebugMessage(-1, 3.0f, FColor::Red, TEXT("Recording started"));
	}
}

void UViewportController::StopRecording()
{
	if (!bIsRecording)
	{
		UE_LOG(LogTemp, Warning, TEXT("ViewportController: Not currently recording"));
		return;
	}

	APlayerController* PC = GetPlayerController();
	if (!PC)
	{
		UE_LOG(LogTemp, Error, TEXT("ViewportController: PlayerController is null, cannot stop recording"));
		return;
	}

	// 녹화 중지
	PC->ConsoleCommand(TEXT("StopMovieCapture"));

	bIsRecording = false;

	UE_LOG(LogTemp, Log, TEXT("ViewportController: Video recording stopped"));

	// Observer Pattern: View에게 녹화 상태 변경 알림
	OnRecordingStateChanged.Broadcast(bIsRecording);

	// 화면에 메시지 출력
	if (GEngine)
	{
		GEngine->AddOnScreenDebugMessage(-1, 3.0f, FColor::Green, TEXT("Recording stopped - Video saved to Saved/VideoCaptures"));
	}

	// VideoCaptures 폴더 열기
	FString VideoDir = FPaths::ProjectSavedDir() / TEXT("VideoCaptures");

	// 디렉토리가 존재하지 않으면 생성
	IPlatformFile& PlatformFile = FPlatformFileManager::Get().GetPlatformFile();
	if (!PlatformFile.DirectoryExists(*VideoDir))
	{
		PlatformFile.CreateDirectory(*VideoDir);
	}

	// CreateProc를 사용하여 explorer.exe로 폴더 열기
	FString ProjectDir = FPaths::ProjectDir();
	FString VideoPath = TEXT("Saved\\VideoCaptures");
	FString FullVideoPath = FPaths::ConvertRelativePathToFull(ProjectDir / VideoPath);

	// Windows Explorer로 폴더 열기
	FPlatformProcess::CreateProc(TEXT("explorer.exe"), *FullVideoPath, true, false, false, nullptr, 0, nullptr, nullptr);
	UE_LOG(LogTemp, Log, TEXT("ViewportController: Opened folder via CreateProc - %s"), *FullVideoPath);
}

// ==================== Helper Functions ====================

APlayerController* UViewportController::GetPlayerController() const
{
	UWorld* World = GetWorld();
	if (!World)
	{
		return nullptr;
	}

	return UGameplayStatics::GetPlayerController(World, 0);
}
