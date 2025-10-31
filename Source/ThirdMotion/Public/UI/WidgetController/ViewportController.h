#pragma once

#include "CoreMinimal.h"
#include "UI/WidgetController/BaseWidgetController.h"
#include "Data/ViewportTypes.h"
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

/*VideoPath = TEXT("Saved\VideoCaptures");
FPlatformProcess::CreateProc(*VideoPath, TEXT(""), true, false, false, nullptr, 0, *ProjectDir, nullptr);*/
		

UCLASS()
class THIRDMOTION_API UViewportController : public UBaseWidgetController
{
	GENERATED_BODY()

public:
	// ==================== Initialization ====================

	virtual void Init() override;

	// ==================== Camera Control ====================

	// 카메라 뷰 설정
	UFUNCTION(BlueprintCallable, Category = "Viewport Controller")
	void SetCameraView(ECameraView ViewType);

	// ==================== DirectionalLight Control ====================

	// DirectionalLight 초기화
	UFUNCTION(BlueprintCallable, Category = "Viewport Controller")
	void InitializeDirectionalLight();

	// Light Slider 값 변경 처리
	UFUNCTION(BlueprintCallable, Category = "Viewport Controller")
	void OnLightSliderValueChanged(float Value);

	// DirectionalLight 가져오기
	UFUNCTION(BlueprintPure, Category = "Viewport Controller")
	class ADirectionalLight* GetDirectionalLight() const { return DirectionalLight; }

	// ==================== ViewportBox & Eye Button ====================

	// ViewportBox 토글
	UFUNCTION(BlueprintCallable, Category = "Viewport Controller")
	void ToggleViewportBox();

	// ViewportBox 표시 여부
	UFUNCTION(BlueprintPure, Category = "Viewport Controller")
	bool IsViewportBoxVisible() const { return bIsViewportBoxVisible; }

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

	// ViewportBox 가시성 변경 이벤트
	DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnViewportBoxVisibilityChanged, bool, bVisible);
	UPROPERTY(BlueprintAssignable, Category = "Viewport Controller Events")
	FOnViewportBoxVisibilityChanged OnViewportBoxVisibilityChanged;

	// Light Rotation 업데이트 이벤트 (Slider UI 동기화용)
	DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnLightRotationUpdated, float, NormalizedPitch);
	UPROPERTY(BlueprintAssignable, Category = "Viewport Controller Events")
	FOnLightRotationUpdated OnLightRotationUpdated;

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

	// DirectionalLight 참조
	UPROPERTY()
	class ADirectionalLight* DirectionalLight;

	// 이전 라이트 회전값 (변경 감지용)
	FRotator LastLightRotation;

	// ViewportBox 표시 상태
	bool bIsViewportBoxVisible;

	// ==================== Helper Functions ====================

	// 패널 전환 헬퍼 함수
	void SwitchPanel(EViewportPanelType NewPanelType);

	// PlayerController 가져오기
	class APlayerController* GetPlayerController() const;
};
