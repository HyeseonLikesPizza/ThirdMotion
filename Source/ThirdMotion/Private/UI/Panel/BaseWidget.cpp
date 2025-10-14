
#include "UI/Widget/BaseWidget.h"
#include "Kismet/GameplayStatics.h"

void UBaseWidget::NativeConstruct()
{
	Super::NativeConstruct();

}

void UBaseWidget::NativeDestruct()
{
	OnUICleanup();
	Super::NativeDestruct();
}
