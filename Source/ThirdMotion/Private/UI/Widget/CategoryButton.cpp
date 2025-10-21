
#include "UI/Widget/CategoryButton.h"
#include "Components/Button.h"
#include "Components/TextBlock.h"

void UCategoryButton::NativeConstruct()
{
	Super::NativeConstruct();

	if (Button)
	{
		Button->OnClicked.Clear();
		Button->OnClicked.AddDynamic(this, &UCategoryButton::HandleClicked);
	}
	

	if (LabelText)
	{
		LabelText->SetColorAndOpacity(
			bIsActive ? FSlateColor(ActiveColor)
				: FSlateColor(NormalColor));

		if (LabelText->GetText().EqualTo(FText::FromString(FString("Library"))))
		{
			LabelText->SetColorAndOpacity(FSlateColor(LibraryColor));
		}
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
			? FSlateColor(ActiveColor)
			: FSlateColor(NormalColor)
		);
	}
	SetIsEnabled(!bIsActive);
}

void UCategoryButton::HandleClicked()
{
	OnCategoryClicked.Broadcast(CategoryTag);
}