
#include "UI/Widget/ViewportWidget.h"
#include "Components/Slider.h"
#include "Components/Button.h"
#include "Components/TextBlock.h"
#include "Components/WidgetSwitcher.h"
#include "UI/WidgetController/ViewportController.h"
#include "Engine/DirectionalLight.h"
#include "Components/DirectionalLightComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Framework/ThirdMotionPlayerController.h"

void UViewportWidget::NativeConstruct()
{
    Super::NativeConstruct();

    InitializeController();

    // DirectionalLight 찾기
    TArray<AActor*> FoundLights;
    UGameplayStatics::GetAllActorsOfClass(GetWorld(), ADirectionalLight::StaticClass(), FoundLights);
    if (FoundLights.Num() > 0)
    {
        DirectionalLight = Cast<ADirectionalLight>(FoundLights[0]);
        if (DirectionalLight)
        {
            // 초기 회전값 저장
            LastLightRotation = DirectionalLight->GetActorRotation();

            // 슬라이더 초기값 설정 (Pitch: -90 ~ 90 → Slider: 0 ~ 1)
            if (Slider_Light)
            {
                float NormalizedValue = (LastLightRotation.Pitch + 90.0f) / 180.0f;
                Slider_Light->SetValue(NormalizedValue);
            }

            UE_LOG(LogTemp, Log, TEXT("ViewportWidget: DirectionalLight found - Initial Pitch=%f"), LastLightRotation.Pitch);
        }
    }

    // Bindings
    if (Slider_Light)
    {
        Slider_Light->OnValueChanged.AddDynamic(this, &UViewportWidget::OnLightSliderValueChanged);
    }
    if (TimeLight)
    {
        TimeLight->OnClicked.AddDynamic(this, &UViewportWidget::OnLightButtonClicked);
    }
    if (Screen)
    {
        Screen->OnClicked.AddDynamic(this, &UViewportWidget::OnScreenButtonClicked);
    }
    if (Cubic)
    {
        Cubic->OnClicked.AddDynamic(this, &UViewportWidget::OnCubicButtonClicked);
    }
    if (ShootButton)
    {
        ShootButton->OnClicked.AddDynamic(this, &UViewportWidget::OnShootButtonClicked);
    }
    if (VideoButton)
    {
        VideoButton->OnClicked.AddDynamic(this, &UViewportWidget::OnVideoButtonClicked);
    }

    // Camera View Buttons
    if (PerspectiveViewButton)
    {
        PerspectiveViewButton->OnClicked.AddDynamic(this, &UViewportWidget::OnPerspectiveViewButtonClicked);
    }
    if (TopViewButton)
    {
        TopViewButton->OnClicked.AddDynamic(this, &UViewportWidget::OnTopViewButtonClicked);
    }
    if (BottomViewButton)
    {
        BottomViewButton->OnClicked.AddDynamic(this, &UViewportWidget::OnBottomViewButtonClicked);
    }
    if (LeftViewButton)
    {
        LeftViewButton->OnClicked.AddDynamic(this, &UViewportWidget::OnLeftViewButtonClicked);
    }
    if (RightViewButton)
    {
        RightViewButton->OnClicked.AddDynamic(this, &UViewportWidget::OnRightViewButtonClicked);
    }
    if (FrontViewButton)
    {
        FrontViewButton->OnClicked.AddDynamic(this, &UViewportWidget::OnFrontViewButtonClicked);
    }
    if (BackViewButton)
    {
        BackViewButton->OnClicked.AddDynamic(this, &UViewportWidget::OnBackViewButtonClicked);
    }
}

void UViewportWidget::NativeDestruct()
{
    Super::NativeDestruct();

    if (ViewportController)
    {
        ViewportController->OnPanelChanged.RemoveAll(this);
        ViewportController->OnRecordingStateChanged.RemoveAll(this);
    }
}

void UViewportWidget::InitializeController()
{
    if (ViewportController == nullptr)
    {
        ViewportController = NewObject<UViewportController>(this);
        ViewportController->Init();

        // Bind controller events
        ViewportController->OnPanelChanged.AddDynamic(this, &UViewportWidget::OnPanelChanged);
        ViewportController->OnRecordingStateChanged.AddDynamic(this, &UViewportWidget::OnRecordingStateChanged);
    }
}

void UViewportWidget::OnLightSliderValueChanged(float Value)
{
    if (!DirectionalLight) return;

    // Slider 값 (0~1)을 Pitch 각도 (-90~90)로 변환
    float NewPitch = (Value * 180.0f) - 90.0f;

    // 새 Rotation 생성 (Roll, Yaw는 유지)
    FRotator NewRotation = DirectionalLight->GetActorRotation();
    NewRotation.Pitch = NewPitch;

    // 로컬 미리보기 (즉시 반영)
    DirectionalLight->SetActorRotation(NewRotation);

    // PlayerController를 통해 서버 RPC 호출
    if (AThirdMotionPlayerController* PC = Cast<AThirdMotionPlayerController>(GetOwningPlayer()))
    {
        PC->Server_UpdateDirectionalLightRotation(NewRotation);
        UE_LOG(LogTemp, Log, TEXT("ViewportWidget: Slider changed - NewPitch=%f, Sending RPC"), NewPitch);
    }
}

void UViewportWidget::OnLightButtonClicked()
{
    if (ViewportController)
    {
        ViewportController->SwitchToLightPanel();
    }
}

void UViewportWidget::OnScreenButtonClicked()
{
    if (ViewportController)
    {
        ViewportController->SwitchToScreenPanel();
    }
}

void UViewportWidget::OnCubicButtonClicked()
{
    if (ViewportController)
    {
        ViewportController->SwitchToCubicPanel();
    }
}

void UViewportWidget::OnShootButtonClicked()
{
    if (ViewportController)
    {
        ViewportController->TakeScreenshot();
    }
}

void UViewportWidget::OnVideoButtonClicked()
{
    if (ViewportController)
    {
        ViewportController->ToggleRecording();
    }
}

void UViewportWidget::OnPanelChanged(EViewportPanelType NewPanelType)
{
    if (WidgetSwitcher)
    {
        WidgetSwitcher->SetActiveWidgetIndex(static_cast<int32>(NewPanelType));
    }
}

void UViewportWidget::OnRecordingStateChanged(bool bIsRecording)
{
    if (VideoButtonText)
    {
        VideoButtonText->SetText(FText::FromString(bIsRecording ? TEXT("Stop") : TEXT("Record")));
    }
}

void UViewportWidget::OnPerspectiveViewButtonClicked()
{
    if (ViewportController)
    {
        ViewportController->SetCameraView(ECameraView::Perspective);
    }
}

void UViewportWidget::OnTopViewButtonClicked()
{
    if (ViewportController)
    {
        ViewportController->SetCameraView(ECameraView::Top);
    }
}

void UViewportWidget::OnBottomViewButtonClicked()
{
    if (ViewportController)
    {
        ViewportController->SetCameraView(ECameraView::Bottom);
    }
}

void UViewportWidget::OnLeftViewButtonClicked()
{
    if (ViewportController)
    {
        ViewportController->SetCameraView(ECameraView::Left);
    }
}

void UViewportWidget::OnRightViewButtonClicked()
{
    if (ViewportController)
    {
        ViewportController->SetCameraView(ECameraView::Right);
    }
}

void UViewportWidget::OnFrontViewButtonClicked()
{
    if (ViewportController)
    {
        ViewportController->SetCameraView(ECameraView::Front);
    }
}

void UViewportWidget::OnBackViewButtonClicked()
{
    if (ViewportController)
    {
        ViewportController->SetCameraView(ECameraView::Back);
    }
}

void UViewportWidget::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);
}

void UViewportWidget::ReleaseSlateResources(bool bReleaseChildren)
{
	Super::ReleaseSlateResources(bReleaseChildren);
}

void UViewportWidget::OnRep_LightRotation()
{
    // Multicast에서 호출됨 - Slider UI 업데이트
    if (Slider_Light && DirectionalLight)
    {
        float CurrentPitch = DirectionalLight->GetActorRotation().Pitch;
        float NormalizedValue = (CurrentPitch + 90.0f) / 180.0f;

        // 슬라이더 업데이트 (이벤트 발생 방지)
        Slider_Light->SetValue(NormalizedValue);

        UE_LOG(LogTemp, Log, TEXT("ViewportWidget: OnRep_LightRotation - Updated slider to Pitch=%f"), CurrentPitch);
    }
}
