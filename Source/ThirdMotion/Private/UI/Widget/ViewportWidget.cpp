#include "UI/Widget/ViewportWidget.h"
  #include "Components/Slider.h"
  #include "Components/Button.h"
  #include "Components/TextBlock.h"
  #include "Components/WidgetSwitcher.h"
  #include "UI/WidgetController/ViewportController.h"
  #include "Engine/DirectionalLight.h"
  #include "Components/DirectionalLightComponent.h"
  #include "EngineUtils.h"
  #include "Kismet/GameplayStatics.h"

  void UViewportWidget::NativeConstruct()
  {
      Super::NativeConstruct();

      InitializeController();

      // DirectionalLight 찾기
      FindDirectionalLight();

      // Slider 초기값 설정
      if (Slider_Light)
      {
          Slider_Light->SetValue(0.5f);
          Slider_Light->OnValueChanged.AddDynamic(this, &UViewportWidget::OnLightSliderValueChanged);

          UE_LOG(LogTemp, Warning, TEXT("ViewportWidget: Slider_Light initialized and bound"));

          if (LightTime)
          {
              LightTime->SetText(FText::FromString(SliderValueToTimeString(0.5f)));
          }
      }
      else
      {
          UE_LOG(LogTemp, Error, TEXT("ViewportWidget: Slider_Light is NULL!"));
      }

      // Bindings
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

          ViewportController->OnPanelChanged.AddDynamic(this, &UViewportWidget::OnPanelChanged);
          ViewportController->OnRecordingStateChanged.AddDynamic(this, &UViewportWidget::OnRecordingStateChanged);
      }
  }

  void UViewportWidget::OnLightSliderValueChanged(float Value)
  {
      UE_LOG(LogTemp, Warning, TEXT("========================================"));
      UE_LOG(LogTemp, Warning, TEXT("ViewportWidget: OnLightSliderValueChanged - Value: %f"), Value);

      if (!DirectionalLight)
      {
          FindDirectionalLight();
          if (!DirectionalLight)
          {
              UE_LOG(LogTemp, Error, TEXT("ViewportWidget: DirectionalLight NOT FOUND!"));
              return;
          }
      }

      // Slider 값을 Pitch 각도로 변환 (-90 ~ 90)
      float Pitch = FMath::Lerp(-90.0f, 90.0f, Value);

      FRotator CurrentRotation = DirectionalLight->GetActorRotation();
      FRotator NewRotation = CurrentRotation;
      NewRotation.Pitch = Pitch;

      DirectionalLight->SetActorRotation(NewRotation);

      UE_LOG(LogTemp, Warning, TEXT("ViewportWidget: Rotation SET - Pitch: %f, Full: %s"),
          Pitch, *NewRotation.ToString());

      // 밝기 조정
      float BrightnessMultiplier = 1.0f - FMath::Abs(Pitch) / 90.0f;
      float Intensity = FMath::Lerp(0.1f, 10.0f, BrightnessMultiplier);

      if (UDirectionalLightComponent* LightComponent = DirectionalLight->GetComponent())
      {
          LightComponent->SetIntensity(Intensity);
          UE_LOG(LogTemp, Warning, TEXT("ViewportWidget: Intensity: %f"), Intensity);
      }

      if (LightTime)
      {
          FString TimeString = SliderValueToTimeString(Value);
          LightTime->SetText(FText::FromString(TimeString));
      }

      UE_LOG(LogTemp, Warning, TEXT("========================================"));
  }

  void UViewportWidget::FindDirectionalLight()
  {
      if (DirectionalLight)
          return;

      UWorld* World = GetWorld();
      if (!World)
      {
          UE_LOG(LogTemp, Error, TEXT("ViewportWidget: World is NULL!"));
          return;
      }

      UE_LOG(LogTemp, Warning, TEXT("ViewportWidget: Searching for DirectionalLight..."));

      int32 Count = 0;
      for (TActorIterator<ADirectionalLight> It(World); It; ++It)
      {
          ADirectionalLight* Light = *It;
          Count++;

          if (IsValid(Light))
          {
              DirectionalLight = Light;
              UE_LOG(LogTemp, Warning, TEXT("ViewportWidget: FOUND DirectionalLight: %s"), *Light->GetName());
              return;
          }
      }

      UE_LOG(LogTemp, Error, TEXT("ViewportWidget: NO DirectionalLight found! (Total: %d)"), Count);
  }

  FString UViewportWidget::SliderValueToTimeString(float Value) const
  {
      float Hours = FMath::Lerp(6.0f, 20.0f, Value);
      int32 Hour = FMath::FloorToInt(Hours);
      int32 Minute = FMath::FloorToInt((Hours - Hour) * 60.0f);
      return FString::Printf(TEXT("%02d:%02d"), Hour, Minute);
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