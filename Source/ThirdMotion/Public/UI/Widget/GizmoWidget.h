// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "GizmoWidget.generated.h"

class UImage;

UENUM(BlueprintType)
enum class EGizmoWidgetMode : uint8
{
    Translate,
    Rotate,
    Scale
};

UENUM(BlueprintType)
enum class EGizmoWidgetAxis : uint8
{
    None,
    X,
    Y,
    Z
};

UCLASS()
class THIRDMOTION_API UGizmoWidget : public UUserWidget
{
	GENERATED_BODY()

	void Init(class APlayerController* InPC);
    void SetTarget(AActor* InTarget);
    void ClearTarget();

    void SetMode(EGizmoWidgetMode InMode) { Mode = InMode; }
    void SetUseLocalSpace(bool bInLocal)  { bLocal = bInLocal; }
    void SetSnap(bool bEnable, float MoveSnap=10.f, float RotSnapDeg=15.f, float ScaleSnap=0.1f);


protected:
    virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;
    virtual FReply NativeOnMouseButtonDown(const FGeometry& InGeo, const FPointerEvent& InEvent) override;
    virtual FReply NativeOnMouseButtonUp(const FGeometry& InGeo, const FPointerEvent& InEvent) override;
    virtual FReply NativeOnMouseMove(const FGeometry& InGeo, const FPointerEvent& InEvent) override;

    UPROPERTY(meta=(BindWidget))
    UImage* ImgX = nullptr;
    
    UPROPERTY(meta=(BindWidget))
    UImage* ImgY = nullptr;
    
    UPROPERTY(meta=(BindWidget))
    UImage* ImgZ = nullptr;

private:
    // 외부
    UPROPERTY()
    APlayerController* PC = nullptr;
    
    UPROPERTY()
    AActor* Target = nullptr;

    // 상태
    EGizmoWidgetMode Mode = EGizmoWidgetMode::Translate;
    EGizmoWidgetAxis ActiveAxis = EGizmoWidgetAxis::None;
    bool bLocal = true;
    bool bDragging = false;
    bool bUniformScale = false;   // Shift로 균등 스케일
    bool bSnap = false;
    float MoveSnapSize = 10.f; 
    float RotSnapDeg = 15.f;
    float ScaleSnap = 0.1f;

    // 드래그 기준
    FTransform TargetStartXf;
    FVector PivotWorld = FVector::ZeroVector;
    FVector AxisDirWorld = FVector::ForwardVector;
    double AxisT_Grab = 0.0;
    FVector PlaneN = FVector::UpVector;
    FVector RotV0 = FVector::ForwardVector;

    // 유틸
    EGizmoWidgetAxis PickAxisFromWidgetLocal(const FVector2D& LocalPos) const; // Img 영역 히트 테스트
    bool ScreenRay(FVector& OutOrigin, FVector& OutDir) const;
    static bool ClosestPointOnAxisToRay(const FVector& O, const FVector& D, const FVector& R0, const FVector& V, double& OutT);
    static bool IntersectRayPlane(const FVector& R0, const FVector& V, const FVector& P, const FVector& N, FVector& OutHit);
    static double Snap(double v, double step) { return step>KINDA_SMALL_NUMBER ? FMath::GridSnap(v, step) : v; }
    static float  SnapDeg(float d, float step){ return step>KINDA_SMALL_NUMBER ? FMath::GridSnap(d, step) : d; }
};
