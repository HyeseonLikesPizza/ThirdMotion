
#include "UI/MainWidget.h"
#include "Components/Widget.h"

void UMainWidget::NativeConstruct()
{
	Super::NativeConstruct();

}

void UMainWidget::OnPanelVisibilityChanged(EPanelType PanelType, bool bVisible)
{
	ESlateVisibility NewVisibility = bVisible ? ESlateVisibility::Visible : ESlateVisibility::Collapsed;

	switch (PanelType)
	{
		case EPanelType::Library:
			if (LeftPanel)
			{
				LeftPanel->SetVisibility(NewVisibility);
			}
			break;

		case EPanelType::Scene:
		case EPanelType::Properties:
			if (RightPanel)
			{
				RightPanel->SetVisibility(NewVisibility);
			}
			break;

		case EPanelType::Thumbnail:
			if (BottomPanel)
			{
				BottomPanel->SetVisibility(NewVisibility);
			}
		break;
	}
}
