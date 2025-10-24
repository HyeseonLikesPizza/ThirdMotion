#pragma once

#include "CoreMinimal.h"
#include "UI/WidgetController/BaseWidgetController.h"
#include "ViewportController.generated.h"

// Forward declarations
class UTexture2D;

/**
 * ViewportController - Controller 역할 (MVC Pattern)
 *
 * 책임:
 * - ViewportWidget의 모든 비즈니스 로직 처리
 * - Widget Switcher 상태 관리 (Light, Screen, Cubic)
 * - 스크린샷 및 비디오 녹화 로직 처리
 *
 * 역할 분리:
 * - Controller: 비즈니스 로직 및 상태 관리
 * - View (ViewportWidget): UI 표시만 담당
 */

// Widget Switcher 인덱스를 위한 Enum
UENUM(BlueprintType)
enum class EViewportPanelType : uint8
{
	Light = 0 UMETA(DisplayName = "Light"),
	Screen = 1 UMETA(DisplayName = "Screen"),
	Cubic = 2 UMETA(DisplayName = "Cubic")
};

// Observer Pattern: 패널 전환 이벤트
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnPanelChanged, EViewportPanelType, NewPanelType);

// Observer Pattern: 스크린샷 완료 이벤트
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnScreenshotTaken, const FString&, FilePath);

// Observer Pattern: 녹화 상태 변경 이벤트
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnRecordingStateChanged, bool, bIsRecording);

UCLASS()
class THIRDMOTION_API UViewportController : public UBaseWidgetController
{
	GENERATED_BODY()

public:
	// ==================== Initialization ====================

	virtual void Init() override;

	// ==================== Panel Switching ====================

	// Light 패널로 전환
	UFUNCTION(BlueprintCallable, Category = "Viewport Controller")
	void SwitchToLightPanel();

	// Screen 패널로 전환
	UFUNCTION(BlueprintCallable, Category = "Viewport Controller")
	void SwitchToScreenPanel();

	// Cubic 패널로 전환
	UFUNCTION(BlueprintCallable, Category = "Viewport Controller")
	void SwitchToCubicPanel();

	// 현재 활성 패널 타입 반환
	UFUNCTION(BlueprintPure, Category = "Viewport Controller")
	EViewportPanelType GetCurrentPanelType() const { return CurrentPanelType; }

	// ==================== Screenshot & Video Recording ====================

	// 스크린샷 촬영
	UFUNCTION(BlueprintCallable, Category = "Viewport Controller")
	void TakeScreenshot();

	// 녹화 토글 (시작/중지)
	UFUNCTION(BlueprintCallable, Category = "Viewport Controller")
	void ToggleRecording();

	// 녹화 시작
	UFUNCTION(BlueprintCallable, Category = "Viewport Controller")
	void StartRecording();

	// 녹화 중지
	UFUNCTION(BlueprintCallable, Category = "Viewport Controller")
	void StopRecording();

	// 녹화 중 여부 반환
	UFUNCTION(BlueprintPure, Category = "Viewport Controller")
	bool IsRecording() const { return bIsRecording; }

	// ==================== Observer Pattern Events ====================

	// 패널 전환 이벤트
	UPROPERTY(BlueprintAssignable, Category = "Viewport Controller Events")
	FOnPanelChanged OnPanelChanged;

	// 스크린샷 완료 이벤트
	UPROPERTY(BlueprintAssignable, Category = "Viewport Controller Events")
	FOnScreenshotTaken OnScreenshotTaken;

	// 녹화 상태 변경 이벤트
	UPROPERTY(BlueprintAssignable, Category = "Viewport Controller Events")
	FOnRecordingStateChanged OnRecordingStateChanged;

private:
	// ==================== State ====================

	// 현재 활성 패널 타입
	UPROPERTY()
	EViewportPanelType CurrentPanelType;

	// 녹화 중 여부
	UPROPERTY()
	bool bIsRecording;

	// 스크린샷 카운터
	UPROPERTY()
	int32 ScreenshotCounter;

	// ==================== Helper Functions ====================

	// 패널 전환 헬퍼 함수
	void SwitchPanel(EViewportPanelType NewPanelType);

	// PlayerController 가져오기
	class APlayerController* GetPlayerController() const;
};
