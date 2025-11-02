#include "UI/Widget/XYZWidget.h"

#include "Components/Button.h"
#include "Components/SpinBox.h"
#include "Components/TextBlock.h"
#include "UI/WidgetController/XYZWidgetController.h"


void UXYZWidget::NativeOnInitialized()
{
	Super::NativeOnInitialized();

	Controller = NewObject<UXYZWidgetController>(this);
    Controller->Initialize(this);
	
    if (PosX)
    {
	    PosX->OnValueCommitted.AddDynamic(this, &UXYZWidget::HandlePosCommitted);
    }
    if (PosY)
    {
	    PosY->OnValueCommitted.AddDynamic(this, &UXYZWidget::HandlePosCommitted);
    }
    if (PosZ)
    {
	    PosZ->OnValueCommitted.AddDynamic(this, &UXYZWidget::HandlePosCommitted);
    }

    if (RotPitch)
    {
	    RotPitch->OnValueCommitted.AddDynamic(this, &UXYZWidget::HandleRotCommitted);
    }
    if (RotYaw)
    {
	    RotYaw->OnValueCommitted.AddDynamic(this, &UXYZWidget::HandleRotCommitted);
    }
    if (RotRoll)
    {
	    RotRoll->OnValueCommitted.AddDynamic(this, &UXYZWidget::HandleRotCommitted);
    }

    if (ScaleX)
    {
	    ScaleX->OnValueCommitted.AddDynamic(this, &UXYZWidget::HandleScaleCommitted);
    }
    if (ScaleY)
    {
	    ScaleY->OnValueCommitted.AddDynamic(this, &UXYZWidget
	    	::HandleScaleCommitted);
    }
    if (ScaleZ)
    {
	    ScaleZ->OnValueCommitted.AddDynamic(this, &UXYZWidget::HandleScaleCommitted);
    }

    if (SizeX)
    {
	    SizeX->OnValueCommitted.AddDynamic(this, &UXYZWidget::HandleSizeCommitted);
    }
    if (SizeY)
    {
	    SizeY->OnValueCommitted.AddDynamic(this, &UXYZWidget::HandleSizeCommitted);
    }
    if (SizeZ)
    {
	    SizeZ->OnValueCommitted.AddDynamic(this, &UXYZWidget::HandleSizeCommitted);
    }
	

    if (ResetPosBtn)
    {
	    ResetPosBtn->OnClicked.AddDynamic(this, &UXYZWidget::HandleResetPos);
    }
    if (ResetRotBtn)
    {
	    ResetRotBtn->OnClicked.AddDynamic(this, &UXYZWidget::HandleResetRot);
    }
    if (ResetScaleBtn)
    {
	    ResetScaleBtn->OnClicked.AddDynamic(this, &UXYZWidget::HandleResetScale);
    }
	
    if (PosX)
    {
	    PosX->SetMinSliderValue(-100000);
    	PosX->SetMaxSliderValue(100000);
    	PosX->SetDelta(1.0);
    }
    if (RotYaw)
    {
	    RotYaw->SetDelta(1.0);
    	RotYaw->SetMinSliderValue(-180);
    	RotYaw->SetMaxSliderValue(180);
    }
    if (ScaleX)
    {
	    ScaleX->SetDelta(1.0);
    	ScaleX->SetMinSliderValue(0.1);
    	ScaleX->SetMaxSliderValue(1000);
    }

	ToggleControls(false);

}

void UXYZWidget::SetTargetActor(AActor* InActor)
{
	const bool bHasTarget = (InActor != nullptr);
	ToggleControls(bHasTarget);
	
	if (Controller)
		Controller->SetTargetActor(InActor);

	if (!bHasTarget)
	{
		//SetObjectName(FText::FromString(InActor->GetActorNameOrLabel()));
		SetPos(FVector::ZeroVector);
		SetRot(FRotator::ZeroRotator);
		SetScale(FVector(100,100,100)); // 100% 기본
		SetSize(FVector::ZeroVector);
		FSlateApplication::Get().ClearKeyboardFocus(EFocusCause::Cleared);
	}
}

void UXYZWidget::SetPos(const FVector& Cm)
{
	if (PosX) PosX->SetValue(Cm.X);
	if (PosY) PosY->SetValue(Cm.Y);
	if (PosZ) PosZ->SetValue(Cm.Z);
}

void UXYZWidget::SetRot(const FRotator& Deg)
{
	if (RotPitch) RotPitch->SetValue(Deg.Pitch);
	if (RotYaw)   RotYaw->SetValue(Deg.Yaw);
	if (RotRoll)  RotRoll->SetValue(Deg.Roll);
}

void UXYZWidget::SetScale(const FVector& Pct)
{
	if (ScaleX) ScaleX->SetValue(Pct.X);
	if (ScaleY) ScaleY->SetValue(Pct.Y);
	if (ScaleZ) ScaleZ->SetValue(Pct.Z);
}

void UXYZWidget::SetSize(const FVector& Meters)
{
	if (SizeX) SizeX->SetValue(Meters.X);
	if (SizeY) SizeY->SetValue(Meters.Y);
	if (SizeZ) SizeZ->SetValue(Meters.Z);
}

void UXYZWidget::SetObjectName(const FText& Name)
{
	ObjectNameText->SetText(Name);
}

void UXYZWidget::ToggleControls(bool bEnabled)
{
	// Pos
	if (PosX) PosX->SetIsEnabled(bEnabled);
	if (PosY) PosY->SetIsEnabled(bEnabled);
	if (PosZ) PosZ->SetIsEnabled(bEnabled);
	// Rot
	if (RotPitch) RotPitch->SetIsEnabled(bEnabled);
	if (RotYaw)   RotYaw->SetIsEnabled(bEnabled);
	if (RotRoll)  RotRoll->SetIsEnabled(bEnabled);
	// Scale (%)
	if (ScaleX) ScaleX->SetIsEnabled(bEnabled);
	if (ScaleY) ScaleY->SetIsEnabled(bEnabled);
	if (ScaleZ) ScaleZ->SetIsEnabled(bEnabled);
	// Size (m)
	if (SizeX) SizeX->SetIsEnabled(bEnabled);
	if (SizeY) SizeY->SetIsEnabled(bEnabled);
	if (SizeZ) SizeZ->SetIsEnabled(bEnabled);
}

void UXYZWidget::HandlePosCommitted(float Value, ETextCommit::Type CommitMethod)
{
	const FVector V = ReadVec3(PosX,PosY,PosZ);
	Controller->OnPosCommitted(V.X, V.Y, V.Z);
}

void UXYZWidget::HandleRotCommitted(float Value, ETextCommit::Type CommitMethod)
{
	const FVector V = ReadVec3(RotPitch,RotYaw,RotRoll);
	Controller->OnRotCommitted(V.X, V.Y, V.Z);
}

void UXYZWidget::HandleScaleCommitted(float Value, ETextCommit::Type CommitMethod)
{
	const FVector V = ReadVec3(ScaleX,ScaleY,ScaleZ);
	Controller->OnScaleCommitted(V.X, V.Y, V.Z);
}

void UXYZWidget::HandleSizeCommitted(float Value, ETextCommit::Type CommitMethod)
{
	const FVector V = ReadVec3(SizeX,SizeY,SizeZ);
	Controller->OnSizeCommitted(V.X, V.Y, V.Z);
}

void UXYZWidget::HandleResetPos()
{
	Controller->OnPosCommitted(0.f,0.f,0.f);
}

void UXYZWidget::HandleResetRot()
{
	Controller->OnRotCommitted(0.f,0.f,0.f);
}

void UXYZWidget::HandleResetScale()
{
	Controller->OnScaleCommitted(100.f,100.f,100.f);
}

FVector UXYZWidget::ReadVec3(USpinBox* X, USpinBox* Y, USpinBox* Z) const
{
	return FVector(X?X->GetValue():0.f, Y?Y->GetValue():0.f, Z?Z->GetValue():0.f);
}
