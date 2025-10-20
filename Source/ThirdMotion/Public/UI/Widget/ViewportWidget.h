

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

protected:
	//virtual TSharedRef<SWidget> RebuildWidget() override;

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

private:
	
	TSharedPtr<class SOverlay> RootWidget;
	TSharedPtr<class SViewport> ViewportWidget;
	TSharedPtr<class FSceneViewport> SceneViewport;

	
};
