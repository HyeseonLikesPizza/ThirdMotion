
#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "BaseWidget.generated.h"

class UUIManager;

UCLASS(Abstract, Blueprintable)
class THIRDMOTION_API UBaseWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	virtual void NativeConstruct() override;
	virtual void NativeDestruct() override;

	// Override this in child classes for custom initialization
	UFUNCTION(BlueprintImplementableEvent, Category = "UI")
	void OnUIInitialize();

	// Override this in child classes for custom cleanup
	UFUNCTION(BlueprintImplementableEvent, Category = "UI")
	void OnUICleanup();
	
};
