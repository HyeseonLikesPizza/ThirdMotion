
#pragma once

#include "CoreMinimal.h"
#include "UI/Widget/BaseWidget.h"
#include "Components/Border.h"
#include "Components/CanvasPanel.h"
#include "Components/SizeBox.h"
#include "MainWidget.generated.h"

UENUM(BlueprintType)
enum class EPanelType : uint8
{
	Library UMETA(DisplayName = "Library"),
	Scene UMETA(DisplayName = "Scene"),
	Properties UMETA(DisplayName = "Properties"),
	Thumbnail UMETA(DisplayName = "Thumbnail")
};

UCLASS()
class THIRDMOTION_API UMainWidget : public UBaseWidget
{
	GENERATED_BODY()

public:
	virtual void NativeConstruct() override;

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

	UFUNCTION()
	void OnPanelVisibilityChanged(EPanelType PanelType, bool bVisible);
};
