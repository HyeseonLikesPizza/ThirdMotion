
#include "UI/Widget/Mesh/MaterialListCombo.h"
#include "Data/MaterialDataTypes.h"


void UMaterialListCombo::AddMaterialItem(FMaterialEntryRow* InRow)
{
	if (!InRow) return;
	
	Items.Add(MakeShared<FMaterialEntryRow>(*InRow));
	Combo->RefreshOptions();
}

void UMaterialListCombo::SetItems(const TArray<FMaterialEntryRow>& InRows)
{
	Items.Reset(InRows.Num());
	for (const FMaterialEntryRow& Row : InRows)
	{
		Items.Add(MakeShared<FMaterialEntryRow>(Row));
	}

	if (!Combo.IsValid())
	{
		return;
	}

	bBlockSelectionEvent = true;
	Combo->RefreshOptions();

	if (!PendingSelection.IsNull())
	{
		bBlockSelectionEvent = false;
		SelectMaterial(PendingSelection.Get(), /*bBroadcastChange=*/false);
		return;
	}

	if (Items.Num() > 0)
	{
		Combo->SetSelectedItem(Items[0]);
		Current = Items[0];
	}
	else
	{
		Combo->ClearSelection();
		Current.Reset();
	}

	bBlockSelectionEvent = false;
}

void UMaterialListCombo::SelectMaterial(UMaterialInterface* Material, bool bBroadcastChange)
{
	PendingSelection = Material;

	if (!Combo.IsValid())
	{
		Current.Reset();
		return;
	}

	TSharedPtr<FMaterialEntryRow> Match;
	if (Material)
	{
		const FSoftObjectPath MaterialPath(Material);

		for (const TSharedPtr<FMaterialEntryRow>& Row : Items)
		{
			if (!Row.IsValid()) continue;
			if (Row->Kind == EMaterialEntryKind::Asset && Row->AssetRef.IsNull()) continue;
			if (Row->Kind == EMaterialEntryKind::DynamicRecipe && Row->BaseAssetRef.IsNull()) continue;
			
			if (Row->Kind == EMaterialEntryKind::Asset)
			{
				if (Row->AssetRef.Get() == Material || Row->AssetRef.ToSoftObjectPath() == MaterialPath)
				{
					Match = Row;
					break;
				}
			}
			// MID 선택 시에도 로직 구현해야함
		}
	}

	const bool bShouldBroadcast = bBroadcastChange && Material != nullptr;
	bBlockSelectionEvent = !bShouldBroadcast;

	if (Match.IsValid())
	{
		Combo->SetSelectedItem(Match);
		Current = Match;

		if (bShouldBroadcast)
		{
			if (UMaterialInterface* LoadedMaterial = Match->AssetRef.LoadSynchronous())
			{
				OnMaterialPicked.Broadcast(LoadedMaterial);
			}
		}
	}
	else
	{
		Combo->ClearSelection();
		Current.Reset();

		if (bBroadcastChange)
		{
			OnMaterialPicked.Broadcast(nullptr);
		}
	}

	bBlockSelectionEvent = false;
}

void UMaterialListCombo::ClearSelection(bool bBroadcastChange)
{
	PendingSelection.Reset();

	if (!Combo.IsValid())
	{
		Current.Reset();
		if (bBroadcastChange)
		{
			OnMaterialPicked.Broadcast(nullptr);
		}
		return;
	}

	bBlockSelectionEvent = !bBroadcastChange;
	Combo->ClearSelection();
	Current.Reset();

	if (bBroadcastChange)
	{
		OnMaterialPicked.Broadcast(nullptr);
	}

	bBlockSelectionEvent = false;
}

TSharedRef<SWidget> UMaterialListCombo::RebuildWidget()
{
	TSharedRef<SComboBox<TSharedPtr<FMaterialEntryRow>>> ComboRef =
		SAssignNew(Combo, SComboBox<TSharedPtr<FMaterialEntryRow>>)
		.OptionsSource(&Items)
		.OnGenerateWidget_UObject(this, &UMaterialListCombo::GenerateItem)
		.OnSelectionChanged_UObject(this, &UMaterialListCombo::OnChanged)
		[
			SNew(STextBlock)
			.Text_UObject(this, &UMaterialListCombo::GetSelectedLabel)
		];

	Combo->RefreshOptions();

	if (!PendingSelection.IsNull())
	{
		SelectMaterial(PendingSelection.Get(), /*bBroadcastChange=*/false);
	}
	else if (Items.Num() > 0)
	{
		bBlockSelectionEvent = true;
		Combo->SetSelectedItem(Items[0]);
		Current = Items[0];
		bBlockSelectionEvent = false;
	}

	return ComboRef;
}

void UMaterialListCombo::ReleaseSlateResources(bool bReleaseChildren)
{
	Super::ReleaseSlateResources(bReleaseChildren);
	Combo.Reset();
}

TSharedRef<SWidget> UMaterialListCombo::GenerateItem(TSharedPtr<FMaterialEntryRow> Row) const
{
	FSlateBrush* Brush = new FSlateBrush();
	Brush->ImageSize = FVector2D(16, 16);

	if (Row.IsValid() && Row->PreviewTexture)
	{
		Brush->SetResourceObject(Row->PreviewTexture);
	}

	return SNew(SHorizontalBox)
	+ SHorizontalBox::Slot().AutoWidth().VAlign(VAlign_Center).Padding(2, 0)
	  [
		  SNew(SImage).Image(Brush)
	  ]
	+ SHorizontalBox::Slot().AutoWidth().VAlign(VAlign_Center).Padding(6, 0)
	  [
		  SNew(STextBlock).Text(FText::FromName(Row->EntryName))
	  ];
}

void UMaterialListCombo::OnChanged(TSharedPtr<FMaterialEntryRow> NewSel, ESelectInfo::Type)
{
	Current = NewSel;

	if (bBlockSelectionEvent)
	{
		return;
	}

	if (Current.IsValid() && !Current->AssetRef.IsNull())
	{
		if (UMaterialInterface* Material = Current->AssetRef.LoadSynchronous())
		{
			OnMaterialPicked.Broadcast(Material);
		}
	}
	else
	{
		OnMaterialPicked.Broadcast(nullptr);
	}
}

FText UMaterialListCombo::GetSelectedLabel() const
{
	return Current.IsValid() ? FText::FromName(Current->EntryName) : FText::FromString(TEXT("None"));
}
