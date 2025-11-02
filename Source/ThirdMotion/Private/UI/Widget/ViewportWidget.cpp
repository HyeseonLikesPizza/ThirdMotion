
#include "UI/Widget/ViewportWidget.h"
#include "Components/Slider.h"
#include "Components/Button.h"
#include "Components/TextBlock.h"
#include "Components/WidgetSwitcher.h"
#include "UI/WidgetController/ViewportController.h"

void UViewportWidget::NativeConstruct()
{
    Super::NativeConstruct();

    InitializeController();

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
    // Implement light slider logic here
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
}
