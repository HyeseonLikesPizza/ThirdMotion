
#include "UI/Panel/LibraryPanel.h"
#include "GameplayTagContainer.h"
#include "IContentBrowserSingleton.h"
#include "Components/TileView.h"
#include "Data/LibraryItemObject.h"
#include "Framework/ThirdMotionPlayerController.h"
#include "UI/Widget/LibraryCategoryWidget.h"
#include "UI/Widget/LibraryItem.h"
#include "UI/WidgetController/LibraryWidgetController.h"

void ULibraryPanel::NativeConstruct()
{
	Super::NativeConstruct();
	
	TileView->OnEntryWidgetGenerated().AddUObject(this, &ULibraryPanel::HandleEntryGenerated);
	CategoryWidget->OnCrumbChosen.AddDynamic(this, &ULibraryPanel::OnCategoryChosen);
}

void ULibraryPanel::Init(ULibraryWidgetController* Controller)
{
	if (!Controller) return;
	WidgetController = Controller;
	ShowCategories(FGameplayTag::RequestGameplayTag(TEXT("Library")));
	//CategoryWidget->SetController(Controller);
}

void ULibraryPanel::ShowPreset(FGameplayTag Category)
{
	// 카테고리 태그로 프리셋 뽑아오기
	TArray<ULibraryItemObject*> Presets;
	WidgetController->QueryByCategory(Category, Presets);

	TArray<UObject*> AsObj;
	AsObj.Reserve(Presets.Num());
	
	for (auto* I : Presets)
	{
		AsObj.Add(I);
	}
	RebuildTileView(AsObj);
}

void ULibraryPanel::ShowCategories(FGameplayTag Category)
{
	// 카테고리 태그로 프리셋 뽑아오기
	TArray<ULibraryItemObject*> Presets;
	WidgetController->GetDirectChildrenCategories(Category, Presets);

	TArray<UObject*> AsObj;
	AsObj.Reserve(Presets.Num());
	
	for (auto* I : Presets)
	{
		AsObj.Add(I);
	}
	CategoryWidget->SetFromTag(Category);
	RebuildTileView(AsObj);
}

void ULibraryPanel::HandleItemClicked(ULibraryItemObject* Item)
{
	if (Item->Type == ELibraryIconType::Category)
	{
		ShowCategories(Item->Tag);
	}
	else
	{
		// 프리뷰 시작: 커서 추적/스냅은 PC Tick에서
		if (AThirdMotionPlayerController* PC = GetWorld()->GetFirstPlayerController<AThirdMotionPlayerController>())
		{
			PC->StartPlacement(Item->Tag);	
		}
	}
}

void ULibraryPanel::HandleEntryGenerated(UUserWidget& EntryWidget)
{
	if (auto* LibraryItem = Cast<ULibraryItem>(&EntryWidget))
	{
		LibraryItem->OnLibraryItemClicked.AddDynamic(this, &ULibraryPanel::HandleItemClicked);
	}
}

void ULibraryPanel::RebuildTileView(const TArray<UObject*>& Items)
{
	TileView->ClearListItems();
	for (auto* Obj : Items)
	{
		TileView->AddItem(Obj);
	}
	TileView->RequestRefresh();
}

void ULibraryPanel::OnCategoryChosen(FGameplayTag Tag)
{
	CurrentCategoryTag = Tag;
	ShowCategories(Tag);
}
