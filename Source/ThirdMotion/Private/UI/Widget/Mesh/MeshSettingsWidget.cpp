
#include "UI/Widget/Mesh/MeshSettingsWidget.h"
#include "Data/MeshDataRow.h"
#include "Edit/AssetResolver.h"
#include "UI/Widget/Mesh/MaterialListCombo.h"
#include "UI/Widget/Mesh/MeshListCombo.h"
#include "Data/MaterialDataTypes.h"


void UMeshSettingsWidget::SetTargetActor(AActor* NewTargetActor)
{
	TargetActor = NewTargetActor;
	UpdateSelectionFromActor();
}

void UMeshSettingsWidget::NativeConstruct()
{
	Super::NativeConstruct();

	
}

void UMeshSettingsWidget::InitFromResolver(class UAssetResolver* Resolver)
{
	if (!MeshListCombo || !Resolver) return;

	/* ---------- Mesh Setting ---------- */

	TArray<const FMeshDataRow*> Rows;
	Resolver->GetAllStaticMeshRows(Rows);

	// UMeshListCombo는 값 배열을 받도록 만들었으니 복사해서 넘김
	TArray<FMeshDataRow> Copies;
	Copies.Reserve(Rows.Num());
	for (const FMeshDataRow* R : Rows)
		Copies.Add(*R);

	MeshListCombo->SetItems(Copies);
	// 선택 시 메시 교체
	MeshListCombo->OnMeshPicked.AddDynamic(this, &UMeshSettingsWidget::ApplyStaticMesh);


	/* ---------- Material Setting ---------- */
	
	TArray<const FMaterialEntryRow*> MaterialRows;
	Resolver->GetAllStaticMaterialRows(MaterialRows);

	// UMaterialListCombo는 값 배열을 받도록 만들었으니 복사해서 넘김
	TArray<FMaterialEntryRow> MCopies;
	MCopies.Reserve(MCopies.Num());
	for (const FMaterialEntryRow* R : MaterialRows)
		MCopies.Add(*R);

	MaterialListCombo->SetItems(MCopies);
	// 선택 시 메시 교체
	MaterialListCombo->OnMaterialPicked.AddDynamic(this, &UMeshSettingsWidget::ApplyMaterial);
}

void UMeshSettingsWidget::ApplyStaticMesh(UStaticMesh* NewMesh)
{
	if (!TargetActor.IsValid() || !NewMesh) return;

	if (UStaticMeshComponent* Comp = TargetActor->FindComponentByClass<UStaticMeshComponent>())
		Comp->SetStaticMesh(NewMesh);
}

void UMeshSettingsWidget::ApplyMaterial(UMaterialInterface* NewMaterial)
{
	if (!TargetActor.IsValid() || !NewMaterial) return;

	if (UStaticMeshComponent* Comp = TargetActor->FindComponentByClass<UStaticMeshComponent>())
		Comp->SetMaterial(0, NewMaterial);
}

void UMeshSettingsWidget::UpdateSelectionFromActor()
{
	if (!MeshListCombo || !MaterialListCombo) return;

	/* ------------ Static Mesh ------------*/
	
	UStaticMesh* CurrentMesh = nullptr;
	if (TargetActor.IsValid())
	{
		if (UStaticMeshComponent* Comp = TargetActor->FindComponentByClass<UStaticMeshComponent>())
		{
			CurrentMesh = Comp->GetStaticMesh();
		}
	}

	if (CurrentMesh)
	{
		MeshListCombo->SelectMesh(CurrentMesh, /*bBroadcastChange=*/false);
	}
	else
	{
		MeshListCombo->ClearSelection(false);
	}

	/* ------------ Material ------------*/

	UMaterialInterface* CurrentMaterial = nullptr;
	if (TargetActor.IsValid())
	{
		if (UStaticMeshComponent* Comp = TargetActor->FindComponentByClass<UStaticMeshComponent>())
		{
			CurrentMaterial = Comp->GetMaterial(0);
		}
	}

	if (CurrentMaterial)
	{
		MaterialListCombo->SelectMaterial(CurrentMaterial, /*bBroadcastChange=*/false);
	}
	else
	{
		MaterialListCombo->ClearSelection(false);
	}
}
