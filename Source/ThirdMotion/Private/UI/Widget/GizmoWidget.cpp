
#include "UI/Widget/GizmoWidget.h"

#include "Components/Image.h"


void UGizmoWidget::Init(APlayerController* InPC)
{
    PC = InPC;
}

void UGizmoWidget::SetTarget(AActor* InTarget)
{
    Target = InTarget;
    bDragging=false;
    ActiveAxis=EGizmoWidgetAxis::None;
    SetVisibility(Target?ESlateVisibility::HitTestInvisible:ESlateVisibility::Collapsed);
}

void UGizmoWidget::ClearTarget()
{
    SetTarget(nullptr);
}

void UGizmoWidget::SetSnap(bool bEnable,float m,float r,float s)
{
    bSnap=bEnable; MoveSnapSize=m; RotSnapDeg=r; ScaleSnap=s;
}


void UGizmoWidget::NativeTick(const FGeometry& MyGeo, float dt)
{
    Super::NativeTick(MyGeo, dt);
    
}

FReply UGizmoWidget::NativeOnMouseButtonDown(const FGeometry& Geo, const FPointerEvent& E)
{
    if (!Target || !PC) return FReply::Unhandled();

    // 이미지 주변 클릭 좌표(위젯 로컬 좌표)
    const FVector2D Local = Geo.AbsoluteToLocal(E.GetScreenSpacePosition());
    ActiveAxis = PickAxisFromWidgetLocal(Local);
    if (ActiveAxis == EGizmoWidgetAxis::None)
        return FReply::Unhandled();

    bDragging = true;
    bUniformScale = E.IsShiftDown();
    TargetStartXf = Target->GetActorTransform();
    PivotWorld    = TargetStartXf.GetLocation();

    // 축 방향(로컬/월드)
    const FTransform Basis = bLocal ? TargetStartXf : FTransform(Target->GetActorRotation());
    switch (ActiveAxis)
    {
        case EGizmoWidgetAxis::X: AxisDirWorld = Basis.GetUnitAxis(EAxis::X); break;
        case EGizmoWidgetAxis::Y: AxisDirWorld = Basis.GetUnitAxis(EAxis::Y); break;
        case EGizmoWidgetAxis::Z: AxisDirWorld = Basis.GetUnitAxis(EAxis::Z); break;
        default: AxisDirWorld = FVector::ForwardVector; break;
    }

    FVector R0, V; ScreenRay(R0, V);
    if (Mode == EGizmoWidgetMode::Translate || Mode == EGizmoWidgetMode::Scale)
    {
        double t0=0.0;
        if (ClosestPointOnAxisToRay(PivotWorld, AxisDirWorld.GetSafeNormal(), R0, V.GetSafeNormal(), t0))
            AxisT_Grab = t0;
    }
    else // Rotate
    {
        PlaneN = AxisDirWorld.GetSafeNormal();
        FVector hit;
        if (IntersectRayPlane(R0, V.GetSafeNormal(), PivotWorld, PlaneN, hit))
            RotV0 = (hit - PivotWorld).GetSafeNormal();
        else
            RotV0 = FVector::ForwardVector;
    }

    return FReply::Handled().CaptureMouse(TakeWidget());
}

FReply UGizmoWidget::NativeOnMouseButtonUp(const FGeometry& Geo, const FPointerEvent& E)
{
    bDragging = false;
    ActiveAxis = EGizmoWidgetAxis::None;
    return FReply::Handled().ReleaseMouseCapture();
}

FReply UGizmoWidget::NativeOnMouseMove(const FGeometry& Geo, const FPointerEvent& E)
{
    if (!bDragging || !Target || !PC) return FReply::Unhandled();

    FVector R0, V; ScreenRay(R0, V); V.Normalize();

    if (Mode == EGizmoWidgetMode::Translate)
    {
        double tNow=0.0;
        if (ClosestPointOnAxisToRay(PivotWorld, AxisDirWorld, R0, V, tNow))
        {
            double d = tNow - AxisT_Grab;
            if (bSnap) d = Snap(d, MoveSnapSize);

            FTransform Xf = TargetStartXf;
            Xf.AddToTranslation(AxisDirWorld * d);
            Target->SetActorTransform(Xf, false, nullptr, ETeleportType::TeleportPhysics);
        }
    }
    else if (Mode == EGizmoWidgetMode::Rotate)
    {
        FVector hit;
        if (IntersectRayPlane(R0, V, PivotWorld, PlaneN, hit))
        {
            const FVector v = (hit - PivotWorld).GetSafeNormal();
            double ang = FMath::RadiansToDegrees(acos(FMath::Clamp(FVector::DotProduct(RotV0,v),-1.0,1.0)));
            const double s = FVector::DotProduct(FVector::CrossProduct(RotV0,v), PlaneN) >= 0 ? 1.0 : -1.0;
            float angDeg = (float)(ang * s);
            if (bSnap) angDeg = SnapDeg(angDeg, RotSnapDeg);

            const FQuat dq(PlaneN, FMath::DegreesToRadians(angDeg));
            FTransform Xf = TargetStartXf;

            if (bLocal)
                Xf.SetRotation((dq * TargetStartXf.GetRotation()).GetNormalized());
            else
            {
                const FVector L = TargetStartXf.GetLocation() - PivotWorld;
                Xf.SetLocation(PivotWorld + dq.RotateVector(L));
                Xf.SetRotation((dq * TargetStartXf.GetRotation()).GetNormalized());
            }
            Target->SetActorTransform(Xf, false, nullptr, ETeleportType::TeleportPhysics);
        }
    }
    else // Scale
    {
        double tNow=0.0;
        if (ClosestPointOnAxisToRay(PivotWorld, AxisDirWorld, R0, V, tNow))
        {
            double d = tNow - AxisT_Grab;
            if (bSnap) d = Snap(d, ScaleSnap);

            FVector S = TargetStartXf.GetScale3D();
            if (bUniformScale)
            {
                float u = FMath::Max(0.001f, S.GetMin() + (float)d);
                S = FVector(u,u,u);
            }
            else
            {
                if (ActiveAxis==EGizmoWidgetAxis::X) S.X = FMath::Max(0.001f, S.X + (float)d);
                if (ActiveAxis==EGizmoWidgetAxis::Y) S.Y = FMath::Max(0.001f, S.Y + (float)d);
                if (ActiveAxis==EGizmoWidgetAxis::Z) S.Z = FMath::Max(0.001f, S.Z + (float)d);
            }
            FTransform Xf = TargetStartXf; Xf.SetScale3D(S);
            Target->SetActorTransform(Xf, false, nullptr, ETeleportType::TeleportPhysics);
        }
    }
    return FReply::Handled();
}

EGizmoWidgetAxis UGizmoWidget::PickAxisFromWidgetLocal(const FVector2D& P) const
{
    // 간단히 각 이미지의 Geometry로 히트 테스트하는 게 가장 정확하지만,
    // 바인딩 편의상 영역 박스 기준으로 판정:
    auto Hit = [&](UImage* Img)->bool{
        if (!Img) return false;
        const FGeometry G = Img->GetCachedGeometry();
        const FVector2D L = G.AbsoluteToLocal(P);
        return FSlateRect(FVector2D::ZeroVector, G.GetLocalSize()).ContainsPoint(L);
    };
    if (Hit(ImgX)) return EGizmoWidgetAxis::X;
    if (Hit(ImgY)) return EGizmoWidgetAxis::Y;
    if (Hit(ImgZ)) return EGizmoWidgetAxis::Z;
    return EGizmoWidgetAxis::None;
}

bool UGizmoWidget::ScreenRay(FVector& OutO, FVector& OutD) const
{
    if (!PC) return false;
    float mx,my; PC->GetMousePosition(mx,my);
    return PC->DeprojectScreenPositionToWorld(mx,my,OutO,OutD);
}

bool UGizmoWidget::ClosestPointOnAxisToRay(const FVector& O,const FVector& D,const FVector& R0,const FVector& V,double& t)
{
    const double b = FVector::DotProduct(D,V);
    const FVector w0 = O - R0;
    const double d = FVector::DotProduct(D,w0);
    const double e = FVector::DotProduct(V,w0);
    const double denom = 1 - b*b;
    if (FMath::IsNearlyZero(denom)) return false;
    t = (b*e - d) / denom;
    return true;
}
bool UGizmoWidget::IntersectRayPlane(const FVector& R0, const FVector& V,const FVector& P,const FVector& N,FVector& H)
{
    const double denom = FVector::DotProduct(N,V);
    if (FMath::IsNearlyZero(denom)) return false;
    const double t = FVector::DotProduct(N,(P-R0))/denom;
    if (t<0) return false;
    H = R0 + V*t;
    return true;
}