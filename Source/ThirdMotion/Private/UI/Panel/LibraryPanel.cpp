
#include "UI/Panel/LibraryPanel.h"
#include "GameplayTagContainer.h"
#include "IContentBrowserSingleton.h"
#include "Components/TileView.h"
#include "Data/LibraryItemObject.h"
#include "UI/WidgetController/LibraryWidgetController.h"

void ULibraryPanel::NativeConstruct()
{
	Super::NativeConstruct();

	if (!WidgetController)
	{
		WidgetController = NewObject<ULibraryWidgetController>(this);
		WidgetController->Init();
	}

	TileView->OnItemClicked().AddUObject(this, &ULibraryPanel::HandleItemClicked);
	Init();
}

void ULibraryPanel::Init()
{
	ShowPreset(FGameplayTag::RequestGameplayTag(TEXT("Category")));
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

void ULibraryPanel::HandleItemClicked(UObject* Item)
{
	if (auto* Data = Cast<ULibraryItemObject>(Item))
	{
		if (Data->Type == ELibraryIconType::Category)
		{
			ShowPreset(Data->Tag);
		}
		else
		{
			// 프리뷰 시작: 커서 추적/스냅은 PC Tick에서
			WidgetController->BeginPreview(Data->Tag);
		}
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
