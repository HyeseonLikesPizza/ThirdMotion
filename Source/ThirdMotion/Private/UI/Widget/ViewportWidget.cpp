
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

    // 포커스 가능하도록 설정
    bIsFocusable = true;

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

    // Bindings (바인딩을 먼저 수행)
    if (Slider_Light)
    {
        Slider_Light->OnValueChanged.AddDynamic(this, &UViewportWidget::OnLightSliderValueChanged);
    }
    if (EyeButton)
    {
        EyeButton->OnClicked.AddDynamic(this, &UViewportWidget::OnEyeButtonClicked);
        //EyeButton->SetVisibility(ESlateVisibility::Visible); // 클릭 이벤트 소비 보장
    }
    if (TimeLight)
    {
        TimeLight->OnClicked.AddDynamic(this, &UViewportWidget::OnLightButtonClicked);
        UE_LOG(LogTemp, Warning, TEXT("ViewportWidget: TimeLight button bound"));
    }
    if (Screen)
    {
        Screen->OnClicked.AddDynamic(this, &UViewportWidget::OnScreenButtonClicked);
        UE_LOG(LogTemp, Warning, TEXT("ViewportWidget: Screen button bound"));
    }
    if (Cubic)
    {
        Cubic->OnClicked.AddDynamic(this, &UViewportWidget::OnCubicButtonClicked);
        UE_LOG(LogTemp, Warning, TEXT("ViewportWidget: Cubic button bound"));
    }
    if (ShootButton)
    {
        ShootButton->OnClicked.AddDynamic(this, &UViewportWidget::OnShootButtonClicked);
        //ShootButton->SetVisibility(ESlateVisibility::Visible); // 클릭 이벤트 소비 보장
    }
    if (VideoButton)
    {
        VideoButton->OnClicked.AddDynamic(this, &UViewportWidget::OnVideoButtonClicked);
        //VideoButton->SetVisibility(ESlateVisibility::Visible); // 클릭 이벤트 소비 보장
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
        ViewportController->OnViewportBoxVisibilityChanged.AddDynamic(this, &UViewportWidget::OnViewportBoxVisibilityChanged);
        ViewportController->OnLightRotationUpdated.AddDynamic(this, &UViewportWidget::OnLightRotationUpdated);
    }
}

void UViewportWidget::OnLightSliderValueChanged(float Value)
{
    // Controller를 통해 처리
    if (ViewportController)
    {
        ViewportController->OnLightSliderValueChanged(Value);
    }
}

void UViewportWidget::OnEyeButtonClicked()
{
    // Controller를 통해 상태 토글
    if (ViewportController)
    {
        ViewportController->ToggleViewportBox();
    }
}

void UViewportWidget::OnLightButtonClicked()
{
    UE_LOG(LogTemp, Warning, TEXT("ViewportWidget: Light button clicked"));

    // 디버그: 버튼 상태 확인
    if (TimeLight)
    {
        UE_LOG(LogTemp, Warning, TEXT("  TimeLight Visibility: %d, IsEnabled: %d"),
            static_cast<int32>(TimeLight->GetVisibility()), TimeLight->GetIsEnabled());
    }

    if (ViewportController)
    {
        ViewportController->SwitchToLightPanel();
        UE_LOG(LogTemp, Warning, TEXT("  ViewportController->SwitchToLightPanel() called"));
    }

    // WidgetSwitcher 표시
    if (WidgetSwitcher)
    {
        WidgetSwitcher->SetVisibility(ESlateVisibility::Visible);
        UE_LOG(LogTemp, Warning, TEXT("  WidgetSwitcher set to Visible"));
    }

    // 위젯에 포커스 설정하여 뷰포트 클릭 방지
    SetKeyboardFocus();
}

void UViewportWidget::OnScreenButtonClicked()
{
    UE_LOG(LogTemp, Warning, TEXT("ViewportWidget: Screen button clicked"));

    if (ViewportController)
    {
        ViewportController->SwitchToScreenPanel();
    }

    // WidgetSwitcher 표시
    if (WidgetSwitcher)
    {
        WidgetSwitcher->SetVisibility(ESlateVisibility::Visible);
        UE_LOG(LogTemp, Warning, TEXT("ViewportWidget: WidgetSwitcher set to Visible"));
    }

    // 위젯에 포커스 설정하여 뷰포트 클릭 방지
    SetKeyboardFocus();
}

void UViewportWidget::OnCubicButtonClicked()
{
    UE_LOG(LogTemp, Warning, TEXT("ViewportWidget: Cubic button clicked"));

    if (ViewportController)
    {
        ViewportController->SwitchToCubicPanel();
    }

    // WidgetSwitcher 표시
    if (WidgetSwitcher)
    {
        WidgetSwitcher->SetVisibility(ESlateVisibility::Visible);
        UE_LOG(LogTemp, Warning, TEXT("ViewportWidget: WidgetSwitcher set to Visible"));
    }

    // 위젯에 포커스 설정하여 뷰포트 클릭 방지
    SetKeyboardFocus();
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

void UViewportWidget::OnViewportBoxVisibilityChanged(bool bVisible)
{
    // ViewportBox 가시성 업데이트
    if (ViewportBox)
    {
        ViewportBox->SetVisibility(bVisible ? ESlateVisibility::Visible : ESlateVisibility::Collapsed);
    }

    // 상태 업데이트
    bIsViewportBoxVisible = bVisible;

    // ViewportBox가 숨겨지면 WidgetSwitcher도 숨김
    if (!bVisible && WidgetSwitcher)
    {
        WidgetSwitcher->SetVisibility(ESlateVisibility::Collapsed);
    }

    // EyeImg 변경
    if (EyeImg)
    {
        if (bVisible && EyeIconWhite)
        {
            // ViewportBox가 표시될 때 흰색 아이콘으로 변경
            EyeImg->SetBrushFromTexture(EyeIconWhite);
            UE_LOG(LogTemp, Log, TEXT("ViewportWidget: EyeImg set to white icon"));
        }
        else
        {
            // ViewportBox가 숨겨질 때 기본 아이콘으로 복원
            if (EyeIconDefault)
            {
                EyeImg->SetBrushFromTexture(EyeIconDefault);
            }
            UE_LOG(LogTemp, Log, TEXT("ViewportWidget: EyeImg set to default icon"));
        }
    }

    UE_LOG(LogTemp, Log, TEXT("ViewportWidget: ViewportBox visibility changed - now %s"),
           bVisible ? TEXT("Visible") : TEXT("Hidden"));
}

void UViewportWidget::OnLightRotationUpdated(float NormalizedPitch)
{
    // Slider UI 업데이트
    if (Slider_Light)
    {
        Slider_Light->SetValue(NormalizedPitch);
        UE_LOG(LogTemp, Log, TEXT("ViewportWidget: Slider updated to value=%f"), NormalizedPitch);
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

FReply UViewportWidget::NativeOnMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
    // 부모 클래스에서 처리 (버튼 클릭, 슬라이더 등)
    FReply Reply = Super::NativeOnMouseButtonDown(InGeometry, InMouseEvent);

    // 버튼이나 다른 위젯이 이미 처리했으면 그대로 반환
    if (Reply.IsEventHandled())
    {
        return Reply;
    }

    // WidgetSwitcher가 보이는 상태에서는 빈 공간 클릭을 소비 (뷰포트 클릭 방지)
    if (WidgetSwitcher && WidgetSwitcher->GetVisibility() == ESlateVisibility::Visible)
    {
        UE_LOG(LogTemp, Log, TEXT("ViewportWidget: Consuming click - WidgetSwitcher is visible"));
        return FReply::Handled();
    }

    // ViewportBox가 보이는 상태에서도 빈 공간 클릭 소비
    if (ViewportBox && ViewportBox->GetVisibility() == ESlateVisibility::Visible)
    {
        UE_LOG(LogTemp, Log, TEXT("ViewportWidget: Consuming click - ViewportBox is visible"));
        return FReply::Handled();
    }

    // 그 외에는 이벤트 통과
    return Reply;
}
