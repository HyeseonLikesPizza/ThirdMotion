
#include "UI/Widget/MeshListCombo.h"

#include "Data/MeshDataRow.h"

void UMeshListCombo::SetItems(const TArray<FMeshDataRow>& InRows)
{
	Items.Reset(InRows.Num());
	for (const FMeshDataRow& Row : InRows)
	{
		Items.Add(MakeShared<FMeshDataRow>(Row)); // 복사해서 TSharedPtr로 저장
	}

	if (Combo.IsValid())
	{
		Combo->RefreshOptions();
		if (Items.Num() > 0)
		{
			Combo->SetSelectedItem(Items[0]);
			Current = Items[0];
		}
	}
}

TSharedRef<SWidget> UMeshListCombo::RebuildWidget()
{
	return SAssignNew(Combo, SComboBox<TSharedPtr<FMeshDataRow>>)
		.OptionsSource(&Items)
		.OnGenerateWidget(this, &UMeshListCombo::GenerateItem)
		.OnSelectionChanged(this, &UMeshListCombo::OnChanged)
		[
			SNew(STextBlock).Text(this, &UMeshListCombo::GetSelectedLabel)
		];
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
		Brush->SetResourceObject(Row->PreviewImage);

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
	if (Current.IsValid() && !Current->MeshAssetRef.IsNull())
	{
		UStaticMesh* Mesh = Current->MeshAssetRef.LoadSynchronous();
		OnMeshPicked.Broadcast(Mesh);
	}
}

FText UMeshListCombo::GetSelectedLabel() const
{
	return Current.IsValid() ? FText::FromName(Current->MeshName) : FText::FromString(TEXT("Select Mesh"));
}
