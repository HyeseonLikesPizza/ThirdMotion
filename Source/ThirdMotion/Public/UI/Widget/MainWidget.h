
#pragma once

#include "CoreMinimal.h"
#include "UI/Widget/BaseWidget.h"
#include "Components/Border.h"
#include "Components/CanvasPanel.h"
#include "Components/SizeBox.h"
#include "MainWidget.generated.h"

class UViewportWidget;

UCLASS()
class THIRDMOTION_API UMainWidget : public UBaseWidget
{
	GENERATED_BODY()

public:
	virtual void NativeConstruct() override;
	virtual void NativeDestruct() override;

	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	UUserWidget* TopBar;

	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	UUserWidget* LibraryPanel;

	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	UUserWidget* RightPanel;

	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	UUserWidget* BottomBar;

	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	UBorder* ViewportContainer;

	// Viewport widget instance (public for PlayerController access)
	UPROPERTY(BlueprintReadWrite)
	UViewportWidget* ViewportWidget;

protected:
	// Viewport widget class to spawn
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Viewport")
	TSubclassOf<UViewportWidget> ViewportWidgetClass;

	// Setup 3D viewport in the container
	//void SetupViewport();
};
