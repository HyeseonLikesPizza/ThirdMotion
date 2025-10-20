
#include "UI/Widget/CategoryButton.h"
#include "Components/Button.h"
#include "Components/TextBlock.h"

void UCategoryButton::NativeConstruct()
{
	Super::NativeConstruct();

	Button->OnClicked.Clear();
	Button->OnClicked.AddDynamic(this, &UCategoryButton::HandleClicked);

	if (LabelText)
	{
		LabelText->SetColorAndOpacity(
			bIsActive ? FSlateColor(FLinearColor(0.3f, 0.64f, 1.0f))
				: FSlateColor(FLinearColor(0.85f, 0.85f, 0.85f)));
	}

	SetIsEnabled(!bIsActive);
}

void UCategoryButton::SetText(const FText& InText)
{
	if (LabelText)
		LabelText->SetText(InText);
}

void UCategoryButton::SetActive(bool bNewActive)
{
	bIsActive = bNewActive;
	if (LabelText)
	{
		LabelText->SetColorAndOpacity(
			bIsActive
			? FSlateColor(FLinearColor(0.3f, 0.64f, 1.0f))
			: FSlateColor(FLinearColor(0.85f, 0.85f, 0.85f))
		);
	}
	SetIsEnabled(!bIsActive);
}
