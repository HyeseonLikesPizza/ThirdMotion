
#pragma once

#include "CoreMinimal.h"
#include "BaseWidget.h"
#include "LibraryPanel.generated.h"

class UTileView;
class UBaseWidgetController;

UCLASS()
class THIRDMOTION_API ULibraryPanel : public UBaseWidget
{
	GENERATED_BODY()

public:

	void NativeConstruct() override;
	
	UPROPERTY(meta=(BindWidgetOptional))
	UTileView* TileView;

	void SetController(UBaseWidgetController* InWC);

	
private:

	UPROPERTY()
	UBaseWidgetController* WidgetController;
	
};
