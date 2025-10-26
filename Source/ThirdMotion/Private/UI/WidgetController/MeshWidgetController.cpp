#include "UI/WidgetController/MeshWidgetController.h"
#include "UI/Widget/Mesh/MeshSettingsWidget.h"
#include "UI/Widget/Mesh/MeshListCombo.h"
#include "Edit/AssetResolver.h"
#include "Data/MeshDataRow.h"
#include "Components/StaticMeshComponent.h"
#include "Framework/ThirdMotionPlayerController.h"
#include "GameFramework/Actor.h"
#include "ThirdMotion/ThirdMotion.h"
#include "UI/Widget/Mesh/MaterialListCombo.h"
#include "Data/MaterialDataTypes.h"
#include "Data/MaterialPreviewData.h"
#include "UI/Panel/MaterialGeneratePanel.h"

void UMeshWidgetController::Initialize(UAssetResolver* InResolver)
{
	if (Resolver == InResolver)
	{
		return;
	}

	Resolver = InResolver;

	RefreshList();
	SyncSelectionToActor();
}

void UMeshWidgetController::AttachView(UMeshSettingsWidget* InView)
{
	if (MeshPanel == InView)
	{
		return;
	}

	// 기존 뷰에서 델리게이트 정리
	if (MeshPanel && MeshPanel->MeshListCombo)
	{
		MeshPanel->MeshListCombo->OnMeshPicked.RemoveDynamic(this, &UMeshWidgetController::HandleMeshPicked);
	}

	if (MeshPanel && MeshPanel->MaterialListCombo)
	{
		MeshPanel->MaterialListCombo->OnMaterialPicked.RemoveDynamic(this, &UMeshWidgetController::HandleMaterialPicked);
	}

	MeshPanel = InView;

	if (!MeshPanel)
	{
		return;
	}

	// 뷰 초기화 및 콤보 갱신
	RefreshList();
	MeshPanel->SetTargetActor(TargetActor.Get());
	SyncSelectionToActor();

	// 현재 PC가 있으면 선택 상태를 즉시 반영
	if (PC)
	{
		HandleSelectionChanged(PC->GetSelectedActor());
	}
}

void UMeshWidgetController::BindPlayerController(AThirdMotionPlayerController* InPC)
{
	if (PC == InPC) return;

	if (PC)
	{
		PC->OnActorSelected.RemoveDynamic(this, &UMeshWidgetController::HandleSelectionChanged);
	}

	PC = InPC;

	if (PC)
	{
		PC->OnActorSelected.AddDynamic(this, &UMeshWidgetController::HandleSelectionChanged);
		HandleSelectionChanged(PC->GetSelectedActor());
	}
}

void UMeshWidgetController::BindMaterialPanel(UMaterialGeneratePanel* InView)
{
	if (!InView) return;
	
	if (MaterialPanel)
	{
		MaterialPanel->OnMaterialCreated.RemoveDynamic(this, &UMeshWidgetController::HandleMaterialCreated);
	}

	MaterialPanel = InView;
	
	MaterialPanel->OnMaterialCreated.AddDynamic(this, &UMeshWidgetController::HandleMaterialCreated);
}

void UMeshWidgetController::SetTargetActor(AActor* InActor)
{
	TargetActor = InActor;

	if (MeshPanel)
	{
		MeshPanel->SetTargetActor(InActor);
	}

	SyncSelectionToActor();
}

void UMeshWidgetController::HandleMeshPicked(UStaticMesh* NewMesh)
{
	if (bComboUpdating || !TargetActor.IsValid() || !NewMesh)
	{
		return;
	}

	if (UStaticMeshComponent* StaticMeshComp = TargetActor->FindComponentByClass<UStaticMeshComponent>())
	{
		if (StaticMeshComp->GetStaticMesh() != NewMesh)
		{
			StaticMeshComp->SetStaticMesh(NewMesh);
		}
	}
}

void UMeshWidgetController::HandleMaterialPicked(UMaterialInterface* NewMaterial)
{
	if (bComboUpdating || !TargetActor.IsValid() || !NewMaterial)
	{
		return;
	}

	if (UStaticMeshComponent* StaticMeshComp = TargetActor->FindComponentByClass<UStaticMeshComponent>())
	{
		if (StaticMeshComp->GetMaterial(0) != NewMaterial)
		{
			StaticMeshComp->SetMaterial(0, NewMaterial);
		}
	}
}

void UMeshWidgetController::HandleSelectionChanged(AActor* SelectedActor)
{
	PRINTLOG(TEXT("HandleSelectionChanged"));
	SetTargetActor(SelectedActor);
}

void UMeshWidgetController::HandleMaterialCreated(UMaterialPreviewData* InObject)
{
	FMaterialEntryRow EntryRow;
	EntryRow.EntryName = FName(*InObject->MaterialName);
	CachedMaterialRows.Add(EntryRow);
	
	if (UMaterialListCombo* MaterialCombo = MeshPanel->MaterialListCombo)
	{
		MaterialCombo->SetItems(CachedMaterialRows);	
	}
}

void UMeshWidgetController::RefreshList()
{
	if (!MeshPanel || !Resolver) return;

	/* ----------- Mesh ----------- */

	// Resolver에서 최신 데이터 복사
	CachedMeshRows.Reset();
	TArray<const FMeshDataRow*> Rows;
	Resolver->GetAllStaticMeshRows(Rows);
	CachedMeshRows.Reserve(Rows.Num());
	for (const FMeshDataRow* Row : Rows)
	{
		if (Row)
		{
			CachedMeshRows.Add(*Row);
		}
	}

	if (UMeshListCombo* Combo = MeshPanel->MeshListCombo)
	{
		Combo->OnMeshPicked.RemoveDynamic(this, &UMeshWidgetController::HandleMeshPicked);
		Combo->SetItems(CachedMeshRows);
		Combo->OnMeshPicked.AddDynamic(this, &UMeshWidgetController::HandleMeshPicked);
	}

	/* ----------- Material ----------- */
	
	// Resolver에서 최신 데이터 복사
	CachedMaterialRows.Reset();
	TArray<FMaterialEntryRow> MaterialRows;
	Resolver->GetAllStaticMaterialRows(MaterialRows);
	CachedMaterialRows.Reserve(MaterialRows.Num());
	for (const FMaterialEntryRow Row : MaterialRows)
	{
		CachedMaterialRows.Add(Row);
	}

	if (UMaterialListCombo* MaterialCombo = MeshPanel->MaterialListCombo)
	{
		MaterialCombo->OnMaterialPicked.RemoveDynamic(this, &UMeshWidgetController::HandleMaterialPicked);
		MaterialCombo->SetItems(CachedMaterialRows);
		MaterialCombo->OnMaterialPicked.AddDynamic(this, &UMeshWidgetController::HandleMaterialPicked);
	}
	
}

void UMeshWidgetController::SyncSelectionToActor()
{
	if (!MeshPanel || !MeshPanel->MeshListCombo) return;
	if (!MeshPanel || !MeshPanel->MaterialListCombo) return;

	UStaticMesh* MeshToApply = nullptr;
	UMaterialInterface* MaterialToApply = nullptr;
	
	if (TargetActor.IsValid())
	{
		if (UStaticMeshComponent* StaticMeshComp = TargetActor->FindComponentByClass<UStaticMeshComponent>())
		{
			MeshToApply = StaticMeshComp->GetStaticMesh();
			MaterialToApply = StaticMeshComp->GetMaterial(0);
		}
	}

	ApplySelectionToMeshCombo(MeshToApply);
	ApplySelectionToMaterialCombo(MaterialToApply);
}

void UMeshWidgetController::ApplySelectionToMeshCombo(UStaticMesh* Mesh)
{
	if (!MeshPanel || !MeshPanel->MeshListCombo)
	{
		return;
	}

	bComboUpdating = true;
	if (Mesh)
	{
		MeshPanel->MeshListCombo->SelectMesh(Mesh, /*bBroadcastChange=*/false);
	}
	else
	{
		MeshPanel->MeshListCombo->ClearSelection(false);
	}
	bComboUpdating = false;
}

void UMeshWidgetController::ApplySelectionToMaterialCombo(UMaterialInterface* Material)
{
	if (!MeshPanel || !MeshPanel->MaterialListCombo) return;

	bComboUpdating = true;
	
	if (Material)
	{
		MeshPanel->MaterialListCombo->SelectMaterial(Material, /*bBroadcastChange=*/false);
	}
	else
	{
		MeshPanel->MaterialListCombo->ClearSelection(false);
	}
	bComboUpdating = false;
}

AActor* UMeshWidgetController::ResolveSelectionFromArray(const TArray<AActor*>& SelectedActors) const
{
	for (AActor* Candidate : SelectedActors)
	{
		if (IsValid(Candidate))
		{
			return Candidate;
		}
	}
	return nullptr;
}

void UMeshWidgetController::BeginDestroy()
{
	if (PC)
	{
		PC->OnActorSelected.RemoveDynamic(this, &UMeshWidgetController::HandleSelectionChanged);
		PC = nullptr;
	}

	if (MeshPanel && MeshPanel->MeshListCombo)
	{
		MeshPanel->MeshListCombo->OnMeshPicked.RemoveDynamic(this, &UMeshWidgetController::HandleMeshPicked);
	}

	Super::BeginDestroy();
}
