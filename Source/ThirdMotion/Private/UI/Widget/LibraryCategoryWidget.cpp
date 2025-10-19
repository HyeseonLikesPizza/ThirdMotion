
#include "UI/Widget/LibraryCategoryWidget.h"

#include "Blueprint/WidgetTree.h"
#include "Components/HorizontalBox.h"
#include "Components/TextBlock.h"
#include "UI/Widget/CategoryButton.h"

void ULibraryCategoryWidget::NativeConstruct()
{
	Super::NativeConstruct();
	Rebuild();
}

void ULibraryCategoryWidget::SetController(ULibraryWidgetController* InController)
{
	Controller = InController;
	Controller->
}

void ULibraryCategoryWidget::SetFromTag(FGameplayTag InTag)
{
	CurrentTag = InTag;
	Rebuild();
}

void ULibraryCategoryWidget::SetFromParts(const TArray<FString>& Parts)
{
	// Parts -> FGameplayTag
	FString Joined;
	for (int32 i=0; i<Parts.Num(); ++i)
	{
		if (i > 0) { Joined += TEXT("."); }
		Joined += Parts[i];
	}
	CurrentTag = FGameplayTag::RequestGameplayTag(FName(*Joined), /*ErrorIfNotFound=*/false);
	Rebuild();
}

void ULibraryCategoryWidget::SplitTag(const FGameplayTag& Tag, TArray<FString>& OutParts)
{
	OutParts.Reset();
	if (!Tag.IsValid()) return;
	Tag.ToString().ParseIntoArray(OutParts, TEXT("."), /*CullEmpty=*/true);
}

FGameplayTag ULibraryCategoryWidget::MakePartialTag(const TArray<FString>& Parts, int32 UpToIdx)
{
	FString S;
	for (int32 i=0; i<=UpToIdx && i<Parts.Num(); ++i)
	{
		if (i > 0) S += TEXT(".");
		S += Parts[i];
	}
	return FGameplayTag::RequestGameplayTag(FName(*S), /*ErrorIfNotFound=*/false);
}


void ULibraryCategoryWidget::Rebuild()
{
	if (!BreadcrumbBox) return;

	BreadcrumbBox->ClearChildren();
	Buttons.Empty();

	// 태그를 파츠로 분해
	TArray<FString> Parts;
	SplitTag(CurrentTag, Parts);
	if (Parts.Num() == 0) return;

	const int32 Last = Parts.Num() - 1;
	for (int32 i=0; i<Parts.Num(); ++i)
	{
		const bool bActive = (i == Last);
		const FGameplayTag Partial = MakePartialTag(Parts, i);

		AddCrumb(Partial, Parts[i], bActive);

		// 마지막이 아니면 구분자 넣기
		if (i != Last)
		{
			UTextBlock* Sep = WidgetTree->ConstructWidget<UTextBlock>(UTextBlock::StaticClass());
			Sep->SetText(FText::FromString(SeparatorSymbol));
			Sep->SetColorAndOpacity(FSlateColor(SeparatorColor));
			BreadcrumbBox->AddChildToHorizontalBox(Sep);
		}
	}
}


void ULibraryCategoryWidget::AddCrumb(const FGameplayTag& Tag, const FString& Title, bool bActive)
{
	UCategoryButton* Btn = WidgetTree->ConstructWidget<UCategoryButton>(UCategoryButton::StaticClass());
	Btn->CategoryTag = Tag;
	Btn->SetText(FText::FromString(Title));
	Btn->SetActive(bActive);

	BreadcrumbBox->AddChildToHorizontalBox(Btn);
	Btn->OnCategoryClicked.AddDynamic(this, &ULibraryCategoryWidget::HandleCrumbClicked);
}

void ULibraryCategoryWidget::HandleCrumbClicked(FGameplayTag Tag)
{
	
}
