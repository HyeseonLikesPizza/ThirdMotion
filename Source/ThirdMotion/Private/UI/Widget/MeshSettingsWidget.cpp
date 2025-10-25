
#include "UI/Widget/MeshSettingsWidget.h"
#include "Data/MeshDataRow.h"
#include "Edit/AssetResolver.h"
#include "UI/Widget/MeshListCombo.h"


void UMeshSettingsWidget::SetTargetActor(AActor* NewTargetActor)
{
	TargetActor = NewTargetActor;
}

void UMeshSettingsWidget::InitFromResolver(class UAssetResolver* Resolver)
{
	if (!MeshListCombo || !Resolver) return;

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
}

void UMeshSettingsWidget::ApplyStaticMesh(UStaticMesh* NewMesh)
{
	if (!TargetActor.IsValid() || !NewMesh) return;

	if (UStaticMeshComponent* Comp = TargetActor->FindComponentByClass<UStaticMeshComponent>())
		Comp->SetStaticMesh(NewMesh);
}
