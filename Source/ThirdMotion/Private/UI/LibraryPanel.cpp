
#include "UI/LibraryPanel.h"

#include "Data/LibraryItemObject.h"
#include "UI/WidgetController/LibraryWidgetController.h"

void ULibraryPanel::NativeConstruct()
{
	Super::NativeConstruct();
}

void ULibraryPanel::SetController(UBaseWidgetController* InWC)
{
	WidgetController = InWC;

	TArray<ULibraryItemObject*> Items;
	ULibraryWidgetController* LWC = Cast<ULibraryWidgetController>(WidgetController);

	
	
	//LWC->QueryByCategory()
}
