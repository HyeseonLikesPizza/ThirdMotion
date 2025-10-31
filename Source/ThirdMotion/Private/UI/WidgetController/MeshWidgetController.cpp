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
#include "UI/Panel/MaterialGeneratePanel.h"
#include "Data/MaterialPreviewData.h"
#include "Edit/EditSyncComponent.h"

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
	if (MeshWidget == InView)
	{
		return;
	}

	// 기존 뷰에서 델리게이트 정리
	if (MeshWidget && MeshWidget->MeshListCombo)
	{
		MeshWidget->MeshListCombo->OnMeshPicked.RemoveDynamic(this, &UMeshWidgetController::HandleMeshPicked);
	}

	if (MeshWidget && MeshWidget->MaterialListCombo)
	{
		MeshWidget->MaterialListCombo->OnMaterialPicked.RemoveDynamic(this, &UMeshWidgetController::HandleMaterialPicked);
	}

	MeshWidget = InView;

	if (!MeshWidget)
	{
		return;
	}

	// 뷰 초기화 및 콤보 갱신
	RefreshList();
	MeshWidget->SetTargetActor(TargetActor.Get());
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

void UMeshWidgetController::SetTargetActor(AActor* InActor)
{
	TargetActor = InActor;

	if (MeshWidget)
	{
		MeshWidget->SetTargetActor(InActor);
	}

	SyncSelectionToActor();
}

void UMeshWidgetController::BindMaterialPanel(UMaterialGeneratePanel* Panel)
{
	if (!Panel)
	{
		UE_LOG(LogTemp, Warning, TEXT("MeshWidgetController: BindMaterialPanel called with null Panel"));
		return;
	}

	MaterialWidget = Panel;
	MaterialWidget->OnMaterialCreated.AddDynamic(this, &UMeshWidgetController::HandleMaterialCreated);
}

void UMeshWidgetController::HandleMeshPicked(UStaticMesh* NewMesh)
{
	if (bComboUpdating || !TargetActor.IsValid() || !NewMesh) return;

	if (UStaticMeshComponent* StaticMeshComp = TargetActor->FindComponentByClass<UStaticMeshComponent>())
	{
		if (StaticMeshComp->GetStaticMesh() != NewMesh)
		{
			//StaticMeshComp->SetStaticMesh(NewMesh);
			
			// FPropertyDelta 구성 (네트워크 동기화) 
			if (UEditSyncComponent* Edit = TargetActor->FindComponentByClass<UEditSyncComponent>())
			{
				FPropertyDelta Delta;
				Delta.PropertyTag = FGameplayTag::RequestGameplayTag(TEXT("Property.Mesh.Static"));
				Delta.ObjectPath = FSoftObjectPath(NewMesh);
				PC->Server_RequestApplyProperty(Edit->GetMeta().Guid, Delta);
			}
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
			//StaticMeshComp->SetMaterial(0, NewMaterial);

			// FPropertyDelta 구성 (네트워크 동기화)
			if (UEditSyncComponent* Edit = TargetActor->FindComponentByClass<UEditSyncComponent>())
			{
				FPropertyDelta Delta;
				Delta.PropertyTag = FGameplayTag::RequestGameplayTag(TEXT("Property.Material.SetSlot"));
				Delta.IntParam = 0; // 적용할 슬롯
				Delta.ObjectPath = FSoftObjectPath(NewMaterial);

				PC->Server_RequestApplyProperty(Edit->GetMeta().Guid, Delta);
			}
		}
	}
}

void UMeshWidgetController::HandleSelectionChanged(AActor* SelectedActor)
{
	PRINTLOG(TEXT("HandleSelectionChanged"));
	SetTargetActor(SelectedActor);
}

void UMeshWidgetController::HandleMaterialCreated(UMaterialPreviewData* MatData)
{
	if (!MatData || !MeshWidget || !MeshWidget->MaterialListCombo) return;
	
	FMaterialEntryRow Row;
	Row.EntryName = FName(*MatData->MaterialName);
	Row.BaseAssetRef = MatData->BaseMaterial;

	MeshWidget->MaterialListCombo->AddMaterialItem(&Row);
}

void UMeshWidgetController::RefreshList()
{
	if (!MeshWidget || !Resolver) return;

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

	if (UMeshListCombo* Combo = MeshWidget->MeshListCombo)
	{
		Combo->OnMeshPicked.RemoveDynamic(this, &UMeshWidgetController::HandleMeshPicked);
		Combo->SetItems(CachedMeshRows);
		Combo->OnMeshPicked.AddDynamic(this, &UMeshWidgetController::HandleMeshPicked);
	}

	/* ----------- Material ----------- */
	
	// Resolver에서 최신 데이터 복사
	CachedMaterialRows.Reset();
	TArray<const FMaterialEntryRow*> MaterialRows;
	Resolver->GetAllStaticMaterialRows(MaterialRows);
	CachedMaterialRows.Reserve(MaterialRows.Num());
	for (const FMaterialEntryRow* Row : MaterialRows)
	{
		if (Row)
		{
			CachedMaterialRows.Add(*Row);
		}
	}

	if (UMaterialListCombo* MaterialCombo = MeshWidget->MaterialListCombo)
	{
		MaterialCombo->OnMaterialPicked.RemoveDynamic(this, &UMeshWidgetController::HandleMaterialPicked);
		MaterialCombo->SetItems(CachedMaterialRows);
		MaterialCombo->OnMaterialPicked.AddDynamic(this, &UMeshWidgetController::HandleMaterialPicked);
	}
	
}

void UMeshWidgetController::SyncSelectionToActor()
{
	if (!MeshWidget || !MeshWidget->MeshListCombo) return;
	if (!MeshWidget || !MeshWidget->MaterialListCombo) return;

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
	if (!MeshWidget || !MeshWidget->MeshListCombo)
	{
		return;
	}

	bComboUpdating = true;
	if (Mesh)
	{
		MeshWidget->MeshListCombo->SelectMesh(Mesh, /*bBroadcastChange=*/false);
	}
	else
	{
		MeshWidget->MeshListCombo->ClearSelection(false);
	}
	bComboUpdating = false;
}

void UMeshWidgetController::ApplySelectionToMaterialCombo(UMaterialInterface* Material)
{
	if (!MeshWidget || !MeshWidget->MaterialListCombo) return;

	bComboUpdating = true;
	
	if (Material)
	{
		MeshWidget->MaterialListCombo->SelectMaterial(Material, /*bBroadcastChange=*/false);
	}
	else
	{
		MeshWidget->MaterialListCombo->ClearSelection(false);
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

	if (MeshWidget && MeshWidget->MeshListCombo)
	{
		MeshWidget->MeshListCombo->OnMeshPicked.RemoveDynamic(this, &UMeshWidgetController::HandleMeshPicked);
	}

	Super::BeginDestroy();
}
