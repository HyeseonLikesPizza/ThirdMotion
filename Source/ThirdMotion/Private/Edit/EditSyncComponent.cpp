
#include "Edit/EditSyncComponent.h"
#include "Camera/CameraComponent.h"
#include "Components/LightComponent.h"
#include "Components/WidgetComponent.h"
#include "Components/MultiLineEditableTextBox.h"
#include "Engine/StaticMeshActor.h"
#include "Blueprint/UserWidget.h"
#include "Net/UnrealNetwork.h"
#include "ThirdMotion/ThirdMotion.h"


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
	DOREPLIFETIME(UEditSyncComponent, R_MemoText);
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

void UEditSyncComponent::Server_SetTransform_Implementation(const FTransform& NewWorldTx)
{
	ApplyTransformAuthoritative(NewWorldTx);
}

void UEditSyncComponent::ApplyTransformAuthoritative(const FTransform& NewWorldTx)
{
	if (AActor* Owner = GetOwner())
	{
		FScopedTransaction Tx(NSLOCTEXT("XYZ", "PanelEdit", "XYZ Panel Edit"));
		Owner->Modify();
		Owner->SetActorTransform(NewWorldTx, false, nullptr, ETeleportType::TeleportPhysics);
	}
}

void UEditSyncComponent::Server_SetLightIntensity_Implementation(float NewIntensity)
{
	ApplyLightIntensityAuthoritative(NewIntensity);
}

void UEditSyncComponent::ApplyLightIntensityAuthoritative(float NewIntensity)
{
	check(GetOwner() && GetOwner()->HasAuthority());

	// PropertyDelta 생성
	FPropertyDelta Delta;
	Delta.PropertyTag = FGameplayTag::RequestGameplayTag(FName("Property.Light.Intensity"));
	Delta.Op = EPropOp::SetFloat;
	Delta.FloatParam = NewIntensity;

	// 네이티브에 적용
	ApplyDeltaToNative(Delta);

	// 히스토리에 추가 (복제됨)
	R_PropsAppliedHistory.Add(Delta);

	UE_LOG(LogTemp, Log, TEXT("EditSyncComponent: Light Intensity set to %f"), NewIntensity);
}

void UEditSyncComponent::Server_SetMemoText_Implementation(const FString& NewText)
{
	ApplyMemoTextAuthoritative(NewText);
}

void UEditSyncComponent::ApplyMemoTextAuthoritative(const FString& NewText)
{
	check(GetOwner() && GetOwner()->HasAuthority());

	// R_MemoText 업데이트 (복제되면 OnRep_MemoText 호출됨)
	R_MemoText = NewText;

	// 서버에서도 WidgetComponent 업데이트
	UpdateMemoWidgetText(NewText);

	UE_LOG(LogTemp, Warning, TEXT("EditSyncComponent: Server - Memo Text set to %s"), *NewText);
}

void UEditSyncComponent::OnRep_Meta()
{
}

void UEditSyncComponent::OnRep_MemoText()
{
	UE_LOG(LogTemp, Warning, TEXT("EditSyncComponent::OnRep_MemoText - Called, Text: %s"), *R_MemoText);

	// 클라이언트에서 WidgetComponent 업데이트
	UpdateMemoWidgetText(R_MemoText);
}

void UEditSyncComponent::OnRep_Props()
{
	UE_LOG(LogTemp, Warning, TEXT("EditSyncComponent::OnRep_Props - Called, History count: %d"), R_PropsAppliedHistory.Num());
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

	if (Key == "Property.Light.Color")
	{
		if (ULightComponent* LC = GetLightC())
			LC->SetLightColor(D.ColorParam);
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
	
	if (Key == "Property.Mesh.Static")
	{
		if (UStaticMeshComponent* SMC = GetSMC())
		{
			if (UStaticMesh* NewMesh = Cast<UStaticMesh>(D.ObjectPath.TryLoad()))
			{
				SMC->SetStaticMesh(NewMesh);
			}
		}
		return;
	}
	
	if (Key == "Property.Material.SetSlot")
	{
		if (UStaticMeshComponent* SMC = GetSMC())
		{
			const int32 SlotIndex = D.IntParam;
			if (UMaterialInterface* NewMat = Cast<UMaterialInterface>(D.ObjectPath.TryLoad()))
			{
				SMC->SetMaterial(SlotIndex, NewMat);
			}
		}
		return;
	}

}

void UEditSyncComponent::ApplyAllPropsHistory()
{
	UE_LOG(LogTemp, Warning, TEXT("EditSyncComponent::ApplyAllPropsHistory - START, Count: %d"), R_PropsAppliedHistory.Num());

	for (const FPropertyDelta& D : R_PropsAppliedHistory)
	{
		UE_LOG(LogTemp, Warning, TEXT("EditSyncComponent::ApplyAllPropsHistory - Applying PropertyTag: %s"), *D.PropertyTag.ToString());
		ApplyDeltaToNative(D);
	}

	UE_LOG(LogTemp, Warning, TEXT("EditSyncComponent::ApplyAllPropsHistory - END"));
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
	{
		if (Owner->FindComponentByClass<ULightComponent>())
		{
			PRINTLOG(TEXT("Light Component 존재"));
			return Owner->FindComponentByClass<ULightComponent>();
		}
	}
	PRINTLOG(TEXT("Light Component 존재하지 않음"));
	return nullptr;
}

UCameraComponent* UEditSyncComponent::GetCameraC() const
{
	if (const AActor* Owner = GetOwner())
		return Owner->FindComponentByClass<UCameraComponent>();
	return nullptr;
}

void UEditSyncComponent::UpdateMemoWidgetText(const FString& NewText)
{
	UE_LOG(LogTemp, Warning, TEXT("EditSyncComponent::UpdateMemoWidgetText - START, Text: %s"), *NewText);

	if (AActor* Owner = GetOwner())
	{
		TArray<UWidgetComponent*> WidgetComponents;
		Owner->GetComponents<UWidgetComponent>(WidgetComponents);

		UE_LOG(LogTemp, Warning, TEXT("EditSyncComponent::UpdateMemoWidgetText - WidgetComponents count: %d"), WidgetComponents.Num());

		for (UWidgetComponent* WidgetComp : WidgetComponents)
		{
			if (WidgetComp && WidgetComp->ComponentHasTag(TEXT("Memo")))
			{
				UE_LOG(LogTemp, Warning, TEXT("EditSyncComponent::UpdateMemoWidgetText - Found Memo WidgetComponent"));

				if (UUserWidget* ViewMemo = WidgetComp->GetWidget())
				{
					UE_LOG(LogTemp, Warning, TEXT("EditSyncComponent::UpdateMemoWidgetText - ViewMemo widget found: %s"), *ViewMemo->GetName());

					// ViewMemo의 NoteTextBox 찾기
					if (UMultiLineEditableTextBox* NoteTextBox = Cast<UMultiLineEditableTextBox>(
						ViewMemo->GetWidgetFromName(TEXT("NoteTextBox"))))
					{
						NoteTextBox->SetText(FText::FromString(NewText));
						UE_LOG(LogTemp, Warning, TEXT("EditSyncComponent::UpdateMemoWidgetText - SUCCESS, Applied Text: %s"), *NewText);
					}
					else
					{
						UE_LOG(LogTemp, Error, TEXT("EditSyncComponent::UpdateMemoWidgetText - NoteTextBox NOT FOUND in ViewMemo"));
					}
				}
				else
				{
					UE_LOG(LogTemp, Error, TEXT("EditSyncComponent::UpdateMemoWidgetText - ViewMemo widget is NULL"));
				}
				break;
			}
		}
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("EditSyncComponent::UpdateMemoWidgetText - Owner is NULL"));
	}
}

