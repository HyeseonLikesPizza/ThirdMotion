

#pragma once
#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "UI/WidgetController/ViewportController.h"
#include "ViewportWidget.generated.h"

UCLASS()
class THIRDMOTION_API UViewportWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;
	virtual void ReleaseSlateResources(bool bReleaseChildren) override;
	virtual void NativeConstruct() override;
	virtual void NativeDestruct() override;

	// Slider_Light 바인딩
	UPROPERTY(meta = (BindWidget))
	class USlider* Slider_Light;

	// Slider 값 변경 콜백
	UFUNCTION()
	void OnLightSliderValueChanged(float Value);

	// DirectionalLight 참조
	UPROPERTY(BlueprintReadWrite)
	class ADirectionalLight* DirectionalLight;

	// 이전 라이트 회전값 (변경 감지용)
	FRotator LastLightRotation;

	// DirectionalLight 회전 동기화 (Multicast에서 수동 설정)
	FRotator ReplicatedLightRotation;

	// 슬라이더 업데이트 함수 (Multicast에서 수동 호출)
	void OnRep_LightRotation();

	// ==================== Widget Switcher & Panel Buttons ====================

	// Widget Switcher (Light, Screen, Cubic 패널 전환)
	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	class UWidgetSwitcher* WidgetSwitcher;

	// Light Button
	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	class UButton* TimeLight;

	// Screen Button
	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	class UButton* Screen;

	// Cubic Button
	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	class UButton* Cubic;

	// Light 버튼 클릭 핸들러
	UFUNCTION()
	void OnLightButtonClicked();

	// Screen 버튼 클릭 핸들러
	UFUNCTION()
	void OnScreenButtonClicked();

	// Cubic 버튼 클릭 핸들러
	UFUNCTION()
	void OnCubicButtonClicked();

	// ==================== Screenshot & Video Recording ====================

	// Shoot Button (Screenshot)
	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	class UButton* ShootButton;

	// Video Button (Start/Stop Recording)
	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	class UButton* VideoButton;

	// Video Button Text (녹화 상태 표시)
	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	class UTextBlock* VideoButtonText;

	// Screenshot button click handler
	UFUNCTION()
	void OnShootButtonClicked();

	// Video recording button click handler
	UFUNCTION()
	void OnVideoButtonClicked();

	// ==================== MVC Pattern ====================

	// ViewportController 
	UFUNCTION(BlueprintCallable, Category = "Viewport")
	UViewportController* GetViewportController() const { return ViewportController; }

protected:
	// Observer Pattern: Controller 이벤트 핸들러
	UFUNCTION()
	void OnPanelChanged(EViewportPanelType NewPanelType);

	UFUNCTION()
	void OnRecordingStateChanged(bool bIsRecording);

private:

	TSharedPtr<class SOverlay> RootWidget;
	TSharedPtr<class SViewport> ViewportWidget;
	TSharedPtr<class FSceneViewport> SceneViewport;

	// ==================== MVC Pattern ====================

	// ViewportController
	UPROPERTY()
	UViewportController* ViewportController;

	// 초기화
	void InitializeController();
};
