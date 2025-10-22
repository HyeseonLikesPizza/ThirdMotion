#include "Data/ActorDetailsData.h"
#include "GameFramework/Actor.h"
#include "Components/StaticMeshComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "Engine/StaticMesh.h"
#include "Engine/SkeletalMesh.h"
#include "Materials/MaterialInterface.h"

void UActorDetailsData::UpdateFromActor(AActor* InActor)
{
	SelectedActor = InActor;

	// 기존 데이터 클리어
	ClearProperties();

	if (!SelectedActor)
	{
		ActorName = TEXT("No Actor Selected");
		ActorClassName = TEXT("");
		NotifyDataChanged();
		return;
	}

	// 액터 정보 업데이트
	ActorName = SelectedActor->GetName();
	ActorClassName = SelectedActor->GetClass()->GetName();

	// Mesh Component 수집
	CollectMeshComponents();

	// Observer에게 알림
	NotifyDataChanged();

	UE_LOG(LogTemp, Log, TEXT("ActorDetailsData updated for: %s"), *ActorName);
}

void UActorDetailsData::AddProperty(const FActorPropertyData& Property)
{
	Properties.Add(Property);
}

void UActorDetailsData::ClearProperties()
{
	Properties.Empty();
}

void UActorDetailsData::SetMesh(UMeshComponent* Component, UObject* NewMesh)
{
	if (!Component || !NewMesh)
		return;

	UObject* OldMesh = nullptr;

	// StaticMeshComponent인 경우
	if (UStaticMeshComponent* StaticMeshComp = Cast<UStaticMeshComponent>(Component))
	{
		OldMesh = StaticMeshComp->GetStaticMesh();
		if (UStaticMesh* StaticMesh = Cast<UStaticMesh>(NewMesh))
		{
			StaticMeshComp->SetStaticMesh(StaticMesh);
			UE_LOG(LogTemp, Log, TEXT("Mesh changed: %s -> %s"),
				OldMesh ? *OldMesh->GetName() : TEXT("None"),
				*StaticMesh->GetName());
		}
	}
	// SkeletalMeshComponent인 경우
	else if (USkeletalMeshComponent* SkeletalMeshComp = Cast<USkeletalMeshComponent>(Component))
	{
		OldMesh = SkeletalMeshComp->GetSkeletalMeshAsset();
		if (USkeletalMesh* SkeletalMesh = Cast<USkeletalMesh>(NewMesh))
		{
			SkeletalMeshComp->SetSkeletalMesh(SkeletalMesh);
			UE_LOG(LogTemp, Log, TEXT("Mesh changed: %s -> %s"),
				OldMesh ? *OldMesh->GetName() : TEXT("None"),
				*SkeletalMesh->GetName());
		}
	}

	// Observer에게 알림
	OnMeshChanged.Broadcast(Component, NewMesh, OldMesh);

	// 데이터 재수집
	UpdateFromActor(SelectedActor);
}

void UActorDetailsData::SetMaterial(UMeshComponent* Component, int32 MaterialIndex, UMaterialInterface* NewMaterial)
{
	if (!Component || !NewMaterial)
		return;

	UMaterialInterface* OldMaterial = Component->GetMaterial(MaterialIndex);
	Component->SetMaterial(MaterialIndex, NewMaterial);

	UE_LOG(LogTemp, Log, TEXT("Material[%d] changed: %s -> %s"),
		MaterialIndex,
		OldMaterial ? *OldMaterial->GetName() : TEXT("None"),
		*NewMaterial->GetName());

	// Observer에게 알림
	OnMaterialChanged.Broadcast(Component, MaterialIndex, NewMaterial, OldMaterial);

	// 데이터 재수집
	UpdateFromActor(SelectedActor);
}

void UActorDetailsData::NotifyDataChanged()
{
	OnDataChanged.Broadcast(this);
}

void UActorDetailsData::CollectMeshComponents()
{
	if (!SelectedActor)
		return;

	// StaticMeshComponent 찾기
	TArray<UStaticMeshComponent*> StaticMeshComponents;
	SelectedActor->GetComponents<UStaticMeshComponent>(StaticMeshComponents);

	for (UStaticMeshComponent* MeshComp : StaticMeshComponents)
	{
		if (MeshComp)
		{
			AddStaticMeshProperty(MeshComp);
			AddMaterialProperties(MeshComp);
		}
	}

	// SkeletalMeshComponent 찾기
	TArray<USkeletalMeshComponent*> SkeletalMeshComponents;
	SelectedActor->GetComponents<USkeletalMeshComponent>(SkeletalMeshComponents);

	for (USkeletalMeshComponent* SkeletalMeshComp : SkeletalMeshComponents)
	{
		if (SkeletalMeshComp)
		{
			AddSkeletalMeshProperty(SkeletalMeshComp);
			AddMaterialProperties(SkeletalMeshComp);
		}
	}
}

void UActorDetailsData::AddStaticMeshProperty(UStaticMeshComponent* MeshComponent)
{
	if (!MeshComponent)
		return;

	FActorPropertyData Property;
	Property.PropertyName = TEXT("Static Mesh");
	Property.PropertyValue = MeshComponent->GetStaticMesh();
	Property.PropertyClass = UStaticMesh::StaticClass();
	Property.TargetComponent = MeshComponent;
	Property.bIsMaterialProperty = false;

	AddProperty(Property);
}

void UActorDetailsData::AddSkeletalMeshProperty(USkeletalMeshComponent* SkeletalMeshComponent)
{
	if (!SkeletalMeshComponent)
		return;

	FActorPropertyData Property;
	Property.PropertyName = TEXT("Skeletal Mesh");
	Property.PropertyValue = SkeletalMeshComponent->GetSkeletalMeshAsset();
	Property.PropertyClass = USkeletalMesh::StaticClass();
	Property.TargetComponent = SkeletalMeshComponent;
	Property.bIsMaterialProperty = false;

	AddProperty(Property);
}

void UActorDetailsData::AddMaterialProperties(UMeshComponent* MeshComponent)
{
	if (!MeshComponent)
		return;

	int32 NumMaterials = MeshComponent->GetNumMaterials();

	for (int32 i = 0; i < NumMaterials; i++)
	{
		FActorPropertyData Property;
		Property.PropertyName = FString::Printf(TEXT("Material [%d]"), i);
		Property.PropertyValue = MeshComponent->GetMaterial(i);
		Property.PropertyClass = UMaterialInterface::StaticClass();
		Property.TargetComponent = MeshComponent;
		Property.MaterialIndex = i;
		Property.bIsMaterialProperty = true;

		AddProperty(Property);
	}
}
