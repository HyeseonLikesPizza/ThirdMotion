

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

	// 마우스 클릭 이벤트 가로채기 (뷰포트 클릭 방지)
	virtual FReply NativeOnMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent) override;

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

	// ==================== Camera View Buttons ====================

	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	class UButton* PerspectiveViewButton;

	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	class UButton* TopViewButton;

	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	class UButton* BottomViewButton;

	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	class UButton* LeftViewButton;

	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	class UButton* RightViewButton;

	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	class UButton* FrontViewButton;

	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	class UButton* BackViewButton;

	// ==================== Widget Switcher & Panel Buttons ====================

	// Eye Button (ViewportBox 표시/숨김 토글)
	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	class UButton* EyeButton;

	// Eye Image (Eye 아이콘 이미지)
	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	class UImage* EyeImg;

	// ViewportBox (VerticalBox - TimeLight, Screen, Cubic 버튼 포함)
	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	class UVerticalBox* ViewportBox;

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

	// Eye 버튼 클릭 핸들러 (ViewportBox 토글)
	UFUNCTION()
	void OnEyeButtonClicked();

	// ==================== Camera View Button Handlers ====================

	UFUNCTION()
	void OnPerspectiveViewButtonClicked();

	UFUNCTION()
	void OnTopViewButtonClicked();

	UFUNCTION()
	void OnBottomViewButtonClicked();

	UFUNCTION()
	void OnLeftViewButtonClicked();

	UFUNCTION()
	void OnRightViewButtonClicked();

	UFUNCTION()
	void OnFrontViewButtonClicked();

	UFUNCTION()
	void OnBackViewButtonClicked();

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

	// ==================== Eye Icon Textures ====================

	// Eye 아이콘 텍스처 (기본 상태)
	UPROPERTY()
	class UTexture2D* EyeIconDefault;

	// Eye 아이콘 텍스처 (활성화 상태 - 흰색)
	UPROPERTY()
	class UTexture2D* EyeIconWhite;

	// ViewportBox 표시 상태
	bool bIsViewportBoxVisible;

	// 초기화
	void InitializeController();
};
