
#include "UI/Widget/ViewportWidget.h"
#include "Components/Slider.h"
#include "Components/Button.h"
#include "Components/TextBlock.h"
#include "Components/WidgetSwitcher.h"
#include "Components/VerticalBox.h"
#include "Components/Image.h"
#include "UI/WidgetController/ViewportController.h"
#include "Engine/DirectionalLight.h"
#include "Components/DirectionalLightComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Framework/ThirdMotionPlayerController.h"
#include "Engine/Texture2D.h"

void UViewportWidget::NativeConstruct()
{
    Super::NativeConstruct();

    InitializeController();

    // Eye 아이콘 텍스처 로드
    EyeIconWhite = Cast<UTexture2D>(StaticLoadObject(UTexture2D::StaticClass(), nullptr,
        TEXT("/Game/Assets/cej/icon/icons8-whiteeye.icons8-whiteeye")));

    if (!EyeIconWhite)
    {
        UE_LOG(LogTemp, Warning, TEXT("ViewportWidget: Failed to load EyeIconWhite texture"));
    }

    // EyeImg에 설정된 기본 텍스처 저장
    if (EyeImg && EyeImg->GetBrush().GetResourceObject())
    {
        EyeIconDefault = Cast<UTexture2D>(EyeImg->GetBrush().GetResourceObject());
        if (EyeIconDefault)
        {
            UE_LOG(LogTemp, Log, TEXT("ViewportWidget: Saved default eye icon from Blueprint"));
        }
    }

    // ViewportBox 표시 상태 초기화
    bIsViewportBoxVisible = false;

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

    // Bindings (바인딩을 먼저 수행)
    if (Slider_Light)
    {
        Slider_Light->OnValueChanged.AddDynamic(this, &UViewportWidget::OnLightSliderValueChanged);
    }
    if (EyeButton)
    {
        EyeButton->OnClicked.AddDynamic(this, &UViewportWidget::OnEyeButtonClicked);
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

    // 초기 상태: ViewportBox와 WidgetSwitcher 숨김 (바인딩 이후에 설정)
    if (ViewportBox)
    {
        ViewportBox->SetVisibility(ESlateVisibility::Collapsed);
    }
    if (WidgetSwitcher)
    {
        WidgetSwitcher->SetVisibility(ESlateVisibility::Collapsed);
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

void UViewportWidget::OnEyeButtonClicked()
{
    if (ViewportBox)
    {
        // ViewportBox 가시성 토글
        ESlateVisibility CurrentVisibility = ViewportBox->GetVisibility();
        bool bIsCurrentlyVisible = (CurrentVisibility == ESlateVisibility::Visible ||
                                    CurrentVisibility == ESlateVisibility::SelfHitTestInvisible ||
                                    CurrentVisibility == ESlateVisibility::HitTestInvisible);

        ViewportBox->SetVisibility(bIsCurrentlyVisible ? ESlateVisibility::Collapsed : ESlateVisibility::Visible);

        // 상태 업데이트
        bIsViewportBoxVisible = !bIsCurrentlyVisible;

        // ViewportBox가 숨겨지면 WidgetSwitcher도 숨김
        if (bIsCurrentlyVisible && WidgetSwitcher)
        {
            WidgetSwitcher->SetVisibility(ESlateVisibility::Collapsed);
        }

        // EyeImg 변경
        if (EyeImg)
        {
            if (bIsViewportBoxVisible && EyeIconWhite)
            {
                // ViewportBox가 표시될 때 흰색 아이콘으로 변경
                EyeImg->SetBrushFromTexture(EyeIconWhite);
                UE_LOG(LogTemp, Log, TEXT("ViewportWidget: EyeImg set to white icon"));
            }
            else
            {
                // ViewportBox가 숨겨질 때 기본 아이콘으로 복원
                // Blueprint에서 설정한 기본 이미지 사용 (텍스처를 null로 설정하면 Blueprint 기본값 사용)
                if (EyeIconDefault)
                {
                    EyeImg->SetBrushFromTexture(EyeIconDefault);
                }
                UE_LOG(LogTemp, Log, TEXT("ViewportWidget: EyeImg set to default icon"));
            }
        }

        UE_LOG(LogTemp, Log, TEXT("ViewportWidget: EyeButton clicked - ViewportBox now %s"),
               bIsCurrentlyVisible ? TEXT("Hidden") : TEXT("Visible"));
    }
}

void UViewportWidget::OnLightButtonClicked()
{
    UE_LOG(LogTemp, Log, TEXT("ViewportWidget: Light button clicked"));

    if (ViewportController)
    {
        ViewportController->SwitchToLightPanel();
    }

    // WidgetSwitcher 표시
    if (WidgetSwitcher)
    {
        WidgetSwitcher->SetVisibility(ESlateVisibility::Visible);
        UE_LOG(LogTemp, Log, TEXT("ViewportWidget: WidgetSwitcher set to Visible"));
    }
}

void UViewportWidget::OnScreenButtonClicked()
{
    UE_LOG(LogTemp, Log, TEXT("ViewportWidget: Screen button clicked"));

    if (ViewportController)
    {
        ViewportController->SwitchToScreenPanel();
    }

    // WidgetSwitcher 표시
    if (WidgetSwitcher)
    {
        WidgetSwitcher->SetVisibility(ESlateVisibility::Visible);
        UE_LOG(LogTemp, Log, TEXT("ViewportWidget: WidgetSwitcher set to Visible"));
    }
}

void UViewportWidget::OnCubicButtonClicked()
{
    UE_LOG(LogTemp, Log, TEXT("ViewportWidget: Cubic button clicked"));

    if (ViewportController)
    {
        ViewportController->SwitchToCubicPanel();
    }

    // WidgetSwitcher 표시
    if (WidgetSwitcher)
    {
        WidgetSwitcher->SetVisibility(ESlateVisibility::Visible);
        UE_LOG(LogTemp, Log, TEXT("ViewportWidget: WidgetSwitcher set to Visible"));
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
