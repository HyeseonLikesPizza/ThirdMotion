#include "UI/Widget/MeshListCombo.h"

#include "Data/MeshDataRow.h"
#include "Engine/StaticMesh.h"
#include "Styling/SlateBrush.h"
#include "UObject/SoftObjectPath.h"
#include "Widgets/DeclarativeSyntaxSupport.h"
#include "Widgets/Images/SImage.h"
#include "Widgets/Input/SComboBox.h"
#include "Widgets/SBoxPanel.h"
#include "Widgets/Text/STextBlock.h"

void UMeshListCombo::SetItems(const TArray<FMeshDataRow>& InRows)
{
	Items.Reset(InRows.Num());
	for (const FMeshDataRow& Row : InRows)
	{
		Items.Add(MakeShared<FMeshDataRow>(Row));
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
		SelectMesh(PendingSelection.Get(), /*bBroadcastChange=*/false);
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

void UMeshListCombo::SelectMesh(UStaticMesh* Mesh, bool bBroadcastChange)
{
	PendingSelection = Mesh;

	if (!Combo.IsValid())
	{
		Current.Reset();
		return;
	}

	TSharedPtr<FMeshDataRow> Match;
	if (Mesh)
	{
		const FSoftObjectPath MeshPath(Mesh);

		for (const TSharedPtr<FMeshDataRow>& Row : Items)
		{
			if (!Row.IsValid() || Row->MeshAssetRef.IsNull())
			{
				continue;
			}

			if (Row->MeshAssetRef.Get() == Mesh || Row->MeshAssetRef.ToSoftObjectPath() == MeshPath)
			{
				Match = Row;
				break;
			}
		}
	}

	const bool bShouldBroadcast = bBroadcastChange && Mesh != nullptr;
	bBlockSelectionEvent = !bShouldBroadcast;

	if (Match.IsValid())
	{
		Combo->SetSelectedItem(Match);
		Current = Match;

		if (bShouldBroadcast)
		{
			if (UStaticMesh* LoadedMesh = Match->MeshAssetRef.LoadSynchronous())
			{
				OnMeshPicked.Broadcast(LoadedMesh);
			}
		}
	}
	else
	{
		Combo->ClearSelection();
		Current.Reset();

		if (bBroadcastChange)
		{
			OnMeshPicked.Broadcast(nullptr);
		}
	}

	bBlockSelectionEvent = false;
}

void UMeshListCombo::ClearSelection(bool bBroadcastChange)
{
	PendingSelection.Reset();

	if (!Combo.IsValid())
	{
		Current.Reset();
		if (bBroadcastChange)
		{
			OnMeshPicked.Broadcast(nullptr);
		}
		return;
	}

	bBlockSelectionEvent = !bBroadcastChange;
	Combo->ClearSelection();
	Current.Reset();

	if (bBroadcastChange)
	{
		OnMeshPicked.Broadcast(nullptr);
	}

	bBlockSelectionEvent = false;
}

TSharedRef<SWidget> UMeshListCombo::RebuildWidget()
{
	TSharedRef<SComboBox<TSharedPtr<FMeshDataRow>>> ComboRef =
		SAssignNew(Combo, SComboBox<TSharedPtr<FMeshDataRow>>)
		.OptionsSource(&Items)
		.OnGenerateWidget_UObject(this, &UMeshListCombo::GenerateItem)
		.OnSelectionChanged_UObject(this, &UMeshListCombo::OnChanged)
		[
			SNew(STextBlock)
			.Text_UObject(this, &UMeshListCombo::GetSelectedLabel)
		];

	Combo->RefreshOptions();

	if (!PendingSelection.IsNull())
	{
		SelectMesh(PendingSelection.Get(), /*bBroadcastChange=*/false);
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

void UMeshListCombo::ReleaseSlateResources(bool bReleaseChildren)
{
	Super::ReleaseSlateResources(bReleaseChildren);
	Combo.Reset();
}

TSharedRef<SWidget> UMeshListCombo::GenerateItem(TSharedPtr<FMeshDataRow> Row) const
{
	FSlateBrush* Brush = new FSlateBrush();
	Brush->ImageSize = FVector2D(16, 16);

	if (Row.IsValid() && Row->PreviewImage)
	{
		Brush->SetResourceObject(Row->PreviewImage);
	}

	return SNew(SHorizontalBox)
	+ SHorizontalBox::Slot().AutoWidth().VAlign(VAlign_Center).Padding(2, 0)
	  [
		  SNew(SImage).Image(Brush)
	  ]
	+ SHorizontalBox::Slot().AutoWidth().VAlign(VAlign_Center).Padding(6, 0)
	  [
		  SNew(STextBlock).Text(FText::FromName(Row->MeshName))
	  ];
}

void UMeshListCombo::OnChanged(TSharedPtr<FMeshDataRow> NewSel, ESelectInfo::Type)
{
	Current = NewSel;

	if (bBlockSelectionEvent)
	{
		return;
	}

	if (Current.IsValid() && !Current->MeshAssetRef.IsNull())
	{
		if (UStaticMesh* Mesh = Current->MeshAssetRef.LoadSynchronous())
		{
			OnMeshPicked.Broadcast(Mesh);
		}
	}
	else
	{
		OnMeshPicked.Broadcast(nullptr);
	}
}

FText UMeshListCombo::GetSelectedLabel() const
{
	return Current.IsValid() ? FText::FromName(Current->MeshName) : FText::FromString(TEXT("None"));
}
