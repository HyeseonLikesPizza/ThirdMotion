#include "UI/WidgetController/MeshWidgetController.h"

#include "UI/Widget/MeshSettingsWidget.h"
#include "UI/Widget/MeshListCombo.h"
#include "UI/WidgetController/SceneController.h"
#include "Edit/AssetResolver.h"
#include "Data/MeshDataRow.h"
#include "Components/StaticMeshComponent.h"
#include "Framework/ThirdMotionPlayerController.h"
#include "GameFramework/Actor.h"
#include "ThirdMotion/ThirdMotion.h"

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
	if (View == InView)
	{
		return;
	}

	// 기존 뷰에서 델리게이트 정리
	if (View && View->MeshListCombo)
	{
		View->MeshListCombo->OnMeshPicked.RemoveDynamic(this, &UMeshWidgetController::HandleMeshPicked);
	}

	View = InView;

	if (!View)
	{
		return;
	}

	// 뷰 초기화 및 콤보 갱신
	RefreshList();
	View->SetTargetActor(TargetActor.Get());
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

	if (View)
	{
		View->SetTargetActor(InActor);
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

void UMeshWidgetController::HandleSelectionChanged(AActor* SelectedActor)
{
	SetTargetActor(SelectedActor);
}

void UMeshWidgetController::RefreshList()
{
	if (!View || !Resolver)
	{
		return;
	}

	// Resolver에서 최신 데이터 복사
	CachedRows.Reset();
	TArray<const FMeshDataRow*> Rows;
	Resolver->GetAllStaticMeshRows(Rows);
	CachedRows.Reserve(Rows.Num());
	for (const FMeshDataRow* Row : Rows)
	{
		if (Row)
		{
			CachedRows.Add(*Row);
		}
	}

	if (UMeshListCombo* Combo = View->MeshListCombo)
	{
		Combo->OnMeshPicked.RemoveDynamic(this, &UMeshWidgetController::HandleMeshPicked);
		Combo->SetItems(CachedRows);
		Combo->OnMeshPicked.AddDynamic(this, &UMeshWidgetController::HandleMeshPicked);
	}
}

void UMeshWidgetController::SyncSelectionToActor()
{
	if (!View || !View->MeshListCombo)
	{
		return;
	}

	UStaticMesh* MeshToApply = nullptr;
	if (TargetActor.IsValid())
	{
		if (UStaticMeshComponent* StaticMeshComp = TargetActor->FindComponentByClass<UStaticMeshComponent>())
		{
			MeshToApply = StaticMeshComp->GetStaticMesh();
		}
	}

	ApplySelectionToCombo(MeshToApply);
}

void UMeshWidgetController::ApplySelectionToCombo(UStaticMesh* Mesh)
{
	if (!View || !View->MeshListCombo)
	{
		return;
	}

	bComboUpdating = true;
	if (Mesh)
	{
		View->MeshListCombo->SelectMesh(Mesh, /*bBroadcastChange=*/false);
	}
	else
	{
		View->MeshListCombo->ClearSelection(false);
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

	if (View && View->MeshListCombo)
	{
		View->MeshListCombo->OnMeshPicked.RemoveDynamic(this, &UMeshWidgetController::HandleMeshPicked);
	}

	Super::BeginDestroy();
}
