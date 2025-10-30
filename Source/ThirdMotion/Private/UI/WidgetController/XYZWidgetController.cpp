
#include "UI/WidgetController/XYZWidgetController.h"

#include "Edit/EditSyncComponent.h"
#include "UI/Widget/XYZWidget.h"

void UXYZWidgetController::Initialize(class UUserWidget* InView)
{
	View = Cast<UXYZWidget>(InView);
	if (!View.IsValid()) return;

	if (UWorld* W = InView->GetWorld())
	{
		FTimerHandle T;
		W->GetTimerManager().SetTimer(T,
			FTimerDelegate::CreateUObject(this, &UXYZWidgetController::TickSync),
			0.15f, true);
	}
}

void UXYZWidgetController::SetTargetActor(AActor* InActor)
{
	Target = InActor;
	TickSync();
}

void UXYZWidgetController::OnPosCommitted(float X, float Y, float Z)
{
	if (!Target.IsValid()) return;
	FTransform Tx = Target->GetActorTransform();

	Tx.SetLocation(FVector(X,Y,Z));
	
	PushTransform(Tx);
}

void UXYZWidgetController::OnRotCommitted(float Pitch, float Yaw, float Roll)
{
	if (!Target.IsValid()) return;
	FTransform Tx = Target->GetActorTransform();

	Tx.SetRotation(FRotator(Pitch,Yaw,Roll).Quaternion());
	PushTransform(Tx);
}

void UXYZWidgetController::OnScaleCommitted(float SXpct, float SYpct, float SZpct)
{
	if (!Target.IsValid()) return;
	FTransform Tx = Target->GetActorTransform();
	Tx.SetScale3D(FVector(SXpct, SYpct, SZpct) / 100.f);
	PushTransform(Tx);
}

void UXYZWidgetController::OnSizeCommitted(float SXm, float SYm, float SZm)
{
	if (!Target.IsValid()) return;

	const FVector CurSizeCm = GetWorldSize(Target.Get()); // cm
	FVector k(1,1,1);
	if (CurSizeCm.X > KINDA_SMALL_NUMBER) k.X = (SXm*100.f) / CurSizeCm.X;
	if (CurSizeCm.Y > KINDA_SMALL_NUMBER) k.Y = (SYm*100.f) / CurSizeCm.Y;
	if (CurSizeCm.Z > KINDA_SMALL_NUMBER) k.Z = (SZm*100.f) / CurSizeCm.Z;

	FTransform Tx = Target->GetActorTransform();
	Tx.SetScale3D(Tx.GetScale3D() * k);
	PushTransform(Tx);
}

void UXYZWidgetController::TickSync()
{
	if (!Target.IsValid() || !View.IsValid()) return;

	const FTransform Tx = Target->GetActorTransform();

	// 위치(월드/로컬)
	FVector Pos = Tx.GetLocation();
	
	// 회전
	FRotator Rot = Tx.Rotator();

	const FVector ScalePct = Tx.GetScale3D()*100.f;
	const FVector SizeM = GetWorldSize(Target.Get()) / 100.f;

	View->SetPos(Pos);
	View->SetRot(Rot);
	View->SetScale(ScalePct);
	View->SetSize(SizeM);
}

FVector UXYZWidgetController::GetWorldSize(AActor* A) const
{
	if (!A) return FVector::ZeroVector;

	if (const UStaticMeshComponent* Mesh = A->FindComponentByClass<UStaticMeshComponent>())
	{
		FBoxSphereBounds LocalBounds = Mesh->CalcLocalBounds();
		FVector LocalSize = LocalBounds.BoxExtent * 2.f; // 로컬 공간 크기(cm)
		FVector Scale = Mesh->GetComponentTransform().GetScale3D();
		return LocalSize * Scale; // 월드 크기(cm)
	}
	return FVector::ZeroVector;
}

void UXYZWidgetController::PushTransform(const FTransform& NewWorldTx)
{
	if (!Target.IsValid()) return;
	AActor* A = Target.Get();

	if (UEditSyncComponent* Sync = A->FindComponentByClass<UEditSyncComponent>())
	{
		if (A->GetLocalRole() == ROLE_Authority) Sync->ApplyTransformAuthoritative(NewWorldTx);
		else Sync->Server_SetTransform(NewWorldTx);
	}
}
