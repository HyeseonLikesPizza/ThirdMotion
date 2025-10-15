
#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "UI/Widget/BaseWidget.h"
#include "LibraryPanel.generated.h"

class ULibraryWidgetController;
class ULibraryItemObject;
class UTileView;
class UBaseWidgetController;



UCLASS()
class THIRDMOTION_API ULibraryPanel : public UBaseWidget
{
	GENERATED_BODY()

public:

	virtual void NativeConstruct() override;
	
	UPROPERTY(meta=(BindWidgetOptional))
	UTileView* TileView;

	void Init();

private:

	UPROPERTY()
	ULibraryWidgetController* WidgetController;

	UFUNCTION()
	void HandleItemClicked(UObject* Item);

	void ShowPreset(FGameplayTag Category);
	void RebuildTileView(const TArray<UObject*>& Items);
};
