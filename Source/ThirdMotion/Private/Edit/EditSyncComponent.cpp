
#include "Edit/EditSyncComponent.h"
#include "Camera/CameraComponent.h"
#include "Components/LightComponent.h"
#include "Engine/StaticMeshActor.h"
#include "Net/UnrealNetwork.h"


UEditSyncComponent::UEditSyncComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
	SetIsReplicatedByDefault(true);
}

void UEditSyncComponent::BeginPlay()
{
	Super::BeginPlay();

	
	if (GetOwner() && GetOwner()->HasAuthority())
	{
		if (!R_Meta.Guid.IsValid())
		{
			R_Meta.Guid = FGuid::NewGuid();
		}
	}
	
}

void UEditSyncComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME_CONDITION(UEditSyncComponent, R_Meta, COND_InitialOnly);
	DOREPLIFETIME(UEditSyncComponent, R_PropsAppliedHistory);
}


void UEditSyncComponent::InitMetaFromPreset(const struct FLibraryRow& Row)
{
	check(GetOwner() && GetOwner()->HasAuthority());

	R_Meta.Guid = FGuid::NewGuid();
	R_Meta.PresetTag = Row.PresetTag;
	R_Meta.DisplayName = Row.DisplayName;
}

void UEditSyncComponent::SetMeta_Unsafe(const FEditMeta& InMeta)
{
	check(GetOwner() && GetOwner()->HasAuthority());
	R_Meta = InMeta;
}

void UEditSyncComponent::BuildSave(struct FActorSave& Out) const
{

}

void UEditSyncComponent::ApplySave(const struct FActorSave& In)
{
}

/* 서버 내부용: SceneManager에서만 호출 */
void UEditSyncComponent::ServerApplyPropertyDelta_Internal(const FPropertyDelta& D)
{
	check(GetOwner() && GetOwner()->HasAuthority());

	ApplyDeltaToNative(D);
	R_PropsAppliedHistory.Add(D);
}

void UEditSyncComponent::OnRep_Meta()
{
}

void UEditSyncComponent::OnRep_Props()
{
	ApplyAllPropsHistory();
}

void UEditSyncComponent::ApplyDeltaToNative(const FPropertyDelta& D)
{
	const FName Key = D.PropertyTag.GetTagName();

	if (Key == "Property.Light.Intensity")
	{
		if (ULightComponent* LC = GetLightC())
			LC->SetIntensity(D.FloatParam);
		return;
	}
	if (D.Op == EPropOp::SetTransform)
	{
		if (AActor* Owner = GetOwner())
		{
			Owner->SetActorTransform(D.TransformParam, true, nullptr, ETeleportType::None);
		}
		return;
	}

	// TODO: SpringArm.ArmLength 등 필요한 키 추가
}

void UEditSyncComponent::ApplyAllPropsHistory()
{
	for (const FPropertyDelta& D : R_PropsAppliedHistory)
		ApplyDeltaToNative(D);
}

UStaticMeshComponent* UEditSyncComponent::GetSMC() const
{
	if (const AActor* Owner = GetOwner())
	{
		if (const AStaticMeshActor* SMA = Cast<AStaticMeshActor>(Owner))
			return SMA->GetStaticMeshComponent();
		return Owner->FindComponentByClass<UStaticMeshComponent>();
	}
	return nullptr;
}

ULightComponent* UEditSyncComponent::GetLightC() const
{
	if (const AActor* Owner = GetOwner())
		return Owner->FindComponentByClass<ULightComponent>();
	return nullptr;
}

UCameraComponent* UEditSyncComponent::GetCameraC() const
{
	if (const AActor* Owner = GetOwner())
		return Owner->FindComponentByClass<UCameraComponent>();
	return nullptr;
}

