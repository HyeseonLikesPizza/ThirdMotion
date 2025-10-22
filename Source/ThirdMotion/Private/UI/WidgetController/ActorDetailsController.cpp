#include "UI/WidgetController/ActorDetailsController.h"
#include "Data/ActorDetailsData.h"
#include "Framework/ThirdMotionPlayerController.h"
#include "GameFramework/Actor.h"
#include "Components/StaticMeshComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "Engine/StaticMesh.h"
#include "Engine/SkeletalMesh.h"
#include "Materials/MaterialInterface.h"

void UActorDetailsController::Initialize(UWorld* InWorld, UActorDetailsData* InDetailsData)
{
	World = InWorld;
	DetailsData = InDetailsData;

	// DetailsData가 없으면 생성
	if (!DetailsData && World)
	{
		DetailsData = NewObject<UActorDetailsData>(this);
	}

	UE_LOG(LogTemp, Log, TEXT("ActorDetailsController initialized"));
}

void UActorDetailsController::SetDetailsData(UActorDetailsData* InDetailsData)
{
	DetailsData = InDetailsData;
}

void UActorDetailsController::SelectActor(AActor* InActor)
{
	if (!DetailsData)
	{
		UE_LOG(LogTemp, Warning, TEXT("DetailsData is null!"));
		return;
	}

	// Model 업데이트
	DetailsData->UpdateFromActor(InActor);

	// Observer에게 알림
	NotifyActorSelected(InActor);

	UE_LOG(LogTemp, Log, TEXT("Actor selected: %s"), InActor ? *InActor->GetName() : TEXT("None"));
}

void UActorDetailsController::ClearSelection()
{
	if (!DetailsData)
		return;

	// Model 클리어
	DetailsData->UpdateFromActor(nullptr);

	// Observer에게 알림
	NotifyActorSelected(nullptr);

	UE_LOG(LogTemp, Log, TEXT("Actor selection cleared"));
}

void UActorDetailsController::ChangeMesh(UMeshComponent* Component, UObject* NewMesh)
{
	if (!DetailsData || !Component || !NewMesh)
	{
		UE_LOG(LogTemp, Warning, TEXT("Invalid parameters for ChangeMesh"));
		return;
	}

	// 네트워크 동기화: PlayerController를 통해 RPC 호출
	AActor* TargetActor = Component->GetOwner();
	if (TargetActor && World)
	{
		if (AThirdMotionPlayerController* PC = Cast<AThirdMotionPlayerController>(World->GetFirstPlayerController()))
		{
			bool bIsSkeletalMesh = Cast<USkeletalMeshComponent>(Component) != nullptr;

			// Server RPC 호출 (클라이언트→서버→모든 클라이언트)
			PC->Server_UpdateActorMesh(TargetActor, NewMesh, bIsSkeletalMesh);

			UE_LOG(LogTemp, Log, TEXT("Mesh change RPC sent: %s"), *NewMesh->GetName());
		}
	}

	// Model을 통해 로컬 Mesh 변경 (RPC가 돌아올 때까지 즉시 적용)
	DetailsData->SetMesh(Component, NewMesh);

	// Observer에게 알림
	NotifyPropertyChanged();

	UE_LOG(LogTemp, Log, TEXT("Mesh changed to: %s"), *NewMesh->GetName());
}

void UActorDetailsController::ChangeMaterial(UMeshComponent* Component, int32 MaterialIndex, UMaterialInterface* NewMaterial)
{
	if (!DetailsData || !Component || !NewMaterial)
	{
		UE_LOG(LogTemp, Warning, TEXT("Invalid parameters for ChangeMaterial"));
		return;
	}

	// 네트워크 동기화: PlayerController를 통해 RPC 호출
	AActor* TargetActor = Component->GetOwner();
	if (TargetActor && World)
	{
		if (AThirdMotionPlayerController* PC = Cast<AThirdMotionPlayerController>(World->GetFirstPlayerController()))
		{
			// Server RPC 호출 (클라이언트→서버→모든 클라이언트)
			PC->Server_UpdateActorMaterial(TargetActor, MaterialIndex, NewMaterial);

			UE_LOG(LogTemp, Log, TEXT("Material change RPC sent: Material[%d] = %s"), MaterialIndex, *NewMaterial->GetName());
		}
	}

	// Model을 통해 로컬 Material 변경 (RPC가 돌아올 때까지 즉시 적용)
	DetailsData->SetMaterial(Component, MaterialIndex, NewMaterial);

	// Observer에게 알림
	NotifyPropertyChanged();

	UE_LOG(LogTemp, Log, TEXT("Material[%d] changed to: %s"), MaterialIndex, *NewMaterial->GetName());
}

void UActorDetailsController::OnAssetSelected(UObject* SelectedAsset, UMeshComponent* TargetComponent, bool bIsMaterial, int32 MaterialIndex)
{
	if (!SelectedAsset || !TargetComponent)
	{
		UE_LOG(LogTemp, Warning, TEXT("Invalid asset or component"));
		return;
	}

	if (bIsMaterial)
	{
		// Material 변경
		if (UMaterialInterface* Material = Cast<UMaterialInterface>(SelectedAsset))
		{
			ChangeMaterial(TargetComponent, MaterialIndex, Material);
		}
	}
	else
	{
		// Mesh 변경
		ChangeMesh(TargetComponent, SelectedAsset);
	}
}

void UActorDetailsController::NotifyActorSelected(AActor* Actor)
{
	OnActorSelected.Broadcast(Actor);
}

void UActorDetailsController::NotifyPropertyChanged()
{
	OnPropertyChanged.Broadcast();
}
