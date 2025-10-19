
#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "UI/Widget/BaseWidget.h"
#include "LibraryPanel.generated.h"

class ULibraryCategoryWidget;
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
	ULibraryCategoryWidget* CategoryWidget;
	
	UPROPERTY(meta=(BindWidgetOptional))
	UTileView* TileView;

	void Init(ULibraryWidgetController* Controller);

private:

	UPROPERTY()
	ULibraryWidgetController* WidgetController;

	UFUNCTION()
	void HandleItemClicked(ULibraryItemObject* Item);

	void HandleEntryGenerated(UUserWidget& EntryWidget);

	void ShowPreset(FGameplayTag Category);
	void ShowCategories(FGameplayTag Category);
	void RebuildTileView(const TArray<UObject*>& Items);
};
