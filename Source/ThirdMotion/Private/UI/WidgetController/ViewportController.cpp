#include "UI/WidgetController/ViewportController.h"
#include "Engine/Engine.h"
#include "GameFramework/PlayerController.h"
#include "Kismet/GameplayStatics.h"
#include "HighResScreenshot.h"
#include "Misc/Paths.h"
#include "HAL/PlatformFileManager.h"

void UViewportController::Init()
{
	Super::Init();

	// 초기 상태 설정
	CurrentPanelType = EViewportPanelType::Light; // 기본값: Light 패널
	bIsRecording = false;
	ScreenshotCounter = 0;

	UE_LOG(LogTemp, Log, TEXT("ViewportController: Initialized"));
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
