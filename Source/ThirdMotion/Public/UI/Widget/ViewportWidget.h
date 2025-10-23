

#pragma once
#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "ViewportWidget.generated.h"

UCLASS()
class THIRDMOTION_API UViewportWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;
	virtual void ReleaseSlateResources(bool bReleaseChildren) override;
	virtual void NativeConstruct() override;

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

	// Take screenshot
	UFUNCTION(BlueprintCallable, Category = "Viewport")
	void TakeScreenshot();

	// Start video recording
	UFUNCTION(BlueprintCallable, Category = "Viewport")
	void StartRecording();

	// Stop video recording
	UFUNCTION(BlueprintCallable, Category = "Viewport")
	void StopRecording();

	// Check if currently recording
	UFUNCTION(BlueprintPure, Category = "Viewport")
	bool IsRecording() const { return bIsRecording; }

private:

	TSharedPtr<class SOverlay> RootWidget;
	TSharedPtr<class SViewport> ViewportWidget;
	TSharedPtr<class FSceneViewport> SceneViewport;

	// Recording state
	bool bIsRecording = false;

	// Screenshot counter for unique filenames
	int32 ScreenshotCounter = 0;
};
