

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

protected:
	virtual TSharedRef<SWidget> RebuildWidget() override;

private:
	TSharedPtr<class SOverlay> RootWidget;
	TSharedPtr<class SViewport> ViewportWidget;
	TSharedPtr<class FSceneViewport> SceneViewport;
};
