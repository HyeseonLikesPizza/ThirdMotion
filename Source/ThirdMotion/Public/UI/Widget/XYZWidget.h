
#pragma once

#include "CoreMinimal.h"
#include "BaseWidget.h"
#include "XYZWidget.generated.h"

class UTextBlock;
class UXYZWidgetController;
class USpinBox;
class UCheckBox;
class UButton;
class AActor;

UCLASS()
class THIRDMOTION_API UXYZWidget : public UBaseWidget
{
	GENERATED_BODY()

public:
    UPROPERTY(meta=(BindWidget))
    UTextBlock* ObjectNameText;
    
	// Pos
    UPROPERTY(meta=(BindWidget))
    USpinBox* PosX;
    
    UPROPERTY(meta=(BindWidget))
    USpinBox* PosY;
    
    UPROPERTY(meta=(BindWidget))
    USpinBox* PosZ;
    
    // Rot
    UPROPERTY(meta=(BindWidget))
    USpinBox* RotPitch;
    
    UPROPERTY(meta=(BindWidget))
    USpinBox* RotYaw;
    
    UPROPERTY(meta=(BindWidget))
    USpinBox* RotRoll;
    
    // Scale (%)
    UPROPERTY(meta=(BindWidget))
    USpinBox* ScaleX;
    
    UPROPERTY(meta=(BindWidget))
    USpinBox* ScaleY;
    
    UPROPERTY(meta=(BindWidget))
    USpinBox* ScaleZ;
    
    // Size (m)
    UPROPERTY(meta=(BindWidget))
    USpinBox* SizeX;
    
    UPROPERTY(meta=(BindWidget))
    USpinBox* SizeY;
    
    UPROPERTY(meta=(BindWidget))
    USpinBox* SizeZ;
    

    // Reset 버튼들(선택)
    UPROPERTY(meta=(BindWidgetOptional))
    UButton* ResetPosBtn;
    
    UPROPERTY(meta=(BindWidgetOptional))
    UButton* ResetRotBtn;
    
    UPROPERTY(meta=(BindWidgetOptional))
    UButton* ResetScaleBtn;
    
    UFUNCTION(BlueprintCallable)
    void SetTargetActor(AActor* InActor);
    
    void SetPos(const FVector& Cm);
    void SetRot(const FRotator& Deg);
    void SetScale(const FVector& Pct);
    void SetSize(const FVector& Meters);
    void SetObjectName(const FText& Name);

    void ToggleControls(bool bEnabled); 

protected:
    virtual void NativeOnInitialized() override;

    // SpinBox 콜백
    UFUNCTION()
    void HandlePosCommitted(float Value, ETextCommit::Type CommitMethod);
    
    UFUNCTION()
    void HandleRotCommitted(float Value, ETextCommit::Type CommitMethod);
    
    UFUNCTION()
    void HandleScaleCommitted(float Value, ETextCommit::Type CommitMethod);
    
    UFUNCTION()
    void HandleSizeCommitted(float Value, ETextCommit::Type CommitMethod);
    
    UFUNCTION()
    void HandleResetPos();
    
    UFUNCTION()
    void HandleResetRot();
    
    UFUNCTION()
    void HandleResetScale();

private:
    FVector ReadVec3(USpinBox* X, USpinBox* Y, USpinBox* Z) const;

public:
    UPROPERTY()
    UXYZWidgetController* Controller; 
};
