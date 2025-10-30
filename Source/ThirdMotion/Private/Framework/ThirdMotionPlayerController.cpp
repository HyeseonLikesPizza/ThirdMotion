#include "Framework/ThirdMotionPlayerController.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "GameplayTagContainer.h"
#include "Edit/SceneManager.h"
#include "Blueprint/UserWidget.h"
#include "Blueprint/WidgetBlueprintLibrary.h"
#include "TimerManager.h"
#include "Edit/HighlightComponent.h"
#include "Engine/World.h"
#include "ThirdMotion/ThirdMotion.h"
#include "UI/Widget/MainWidget.h"
#include "UI/Panel/LibraryPanel.h"
#include "UI/Panel/RightPanel.h"
#include "UI/WidgetController/LibraryWidgetController.h"
#include "UI/WidgetController/SceneController.h"
#include "Engine/DirectionalLight.h"
#include "Components/DirectionalLightComponent.h"
#include "Components/MultiLineEditableTextBox.h"
#include "Kismet/GameplayStatics.h"
#include "UI/Widget/ViewportWidget.h"
#include "Camera/PlayerCameraManager.h"
#include "Engine/PostProcessVolume.h"
#include "UI/Widget/SceneListWidget.h"

void AThirdMotionPlayerController::BeginPlay()
{
	Super::BeginPlay();

	// Input Mode, Mouse Cursor 보이기 설정
	bShowMouseCursor = true;
	bEnableClickEvents = true;
	bEnableMouseOverEvents = true;

	FInputModeGameAndUI M;
	M.SetHideCursorDuringCapture(false);
	SetInputMode(M);

	PRINTLOG(TEXT("BeginPlay"));

	// 초기 카메라 위치 및 회전 저장 (Perspective 버튼으로 되돌아갈 뷰)
	APawn* ControlledPawn = GetPawn();
	if (ControlledPawn)
	{
		InitialCameraLocation = ControlledPawn->GetActorLocation();
		InitialCameraRotation = GetControlRotation();
		bHasInitialView = true;
		UE_LOG(LogTemp, Log, TEXT("Initial camera view saved: Location=%s, Rotation=%s"),
			*InitialCameraLocation.ToString(), *InitialCameraRotation.ToString());
	}

	// Library Widget Controller 생성
	if (!LibraryWidgetController)
	{
		LibraryWidgetController = NewObject<ULibraryWidgetController>(this);
		LibraryWidgetController->Init();
	}


	// 메인 Form 생성
	if (MainWidgetClass)
	{
		if (!IsLocalPlayerController()) return;
		MainWidget = CreateWidget<UMainWidget>(this, MainWidgetClass);
		ULibraryPanel* LBWidget = Cast<ULibraryPanel>(MainWidget->LibraryPanel);
		LBWidget->Init(LibraryWidgetController);
		MainWidget->AddToViewport();
	}
}

void AThirdMotionPlayerController::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	// 프리뷰 고스트 상태일 때만 Tick 로직 수행
	if (!bPlacing || !LibraryWidgetController) return;

	FHitResult Hit;
	bool bHit = GetHitResultUnderCursorByChannel(UEngineTypes::ConvertToTraceType(ECC_Visibility), true, Hit);

	FTransform Xf = FTransform::Identity;

	if (bHit)
	{
		FVector Loc = Hit.Location;
		Xf.SetLocation(Loc);
	}

	LibraryWidgetController->UpdatePreviewTransform(Xf);
	LastPreviewXf = Xf;

	
}

void AThirdMotionPlayerController::SetupInputComponent()
{
	Super::SetupInputComponent();

	if (ULocalPlayer* LP = GetLocalPlayer())
	{
		if (UEnhancedInputLocalPlayerSubsystem* Subsys =
			ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(LP))
		{
			Subsys->AddMappingContext(IMC, 0);
		}
	}
	

	if (UEnhancedInputComponent* EIC = Cast<UEnhancedInputComponent>(InputComponent))
	{
		EIC->BindAction(IA_Click, ETriggerEvent::Started, this, &AThirdMotionPlayerController::OnClick);
		EIC->BindAction(IA_Move, ETriggerEvent::Triggered, this, &AThirdMotionPlayerController::MoveCamera);
		EIC->BindAction(IA_MoveUp, ETriggerEvent::Triggered, this, &AThirdMotionPlayerController::MoveCameraUp);
		EIC->BindAction(IA_RMB, ETriggerEvent::Started,  this, &AThirdMotionPlayerController::OnRMB_Pressed);
		EIC->BindAction(IA_RMB, ETriggerEvent::Completed,this, &AThirdMotionPlayerController::OnRMB_Released);
		EIC->BindAction(IA_RMB, ETriggerEvent::Canceled, this, &AThirdMotionPlayerController::OnRMB_Released);
		EIC->BindAction(IA_Look, ETriggerEvent::Triggered, this, &AThirdMotionPlayerController::OnLook);
	}
	
}

AActor* AThirdMotionPlayerController::GetSelectedActor()
{
	return SelectedActor;
}

void AThirdMotionPlayerController::Server_RequestApplyProperty_Implementation(const FGuid& TargetGuid,
	const FPropertyDelta& Delta)
{
	if (!HasAuthority() || !TargetGuid.IsValid()) return;
	if (USceneManager* SceneManager = GetWorld()->GetSubsystem<USceneManager>())
	{
		SceneManager->ApplyPropertyDelta(TargetGuid, Delta);
	}
}

void AThirdMotionPlayerController::StartPlacement(const FGameplayTag& PresetTag)
{
	if (!LibraryWidgetController) return;
	
	CurrentPreset = PresetTag;
	bPlacing = true;
	LibraryWidgetController->BeginPreview(PresetTag);
}

void AThirdMotionPlayerController::StopPlacement(bool bCancel)
{
	if (!LibraryWidgetController) return;
	
	if (bCancel)
		LibraryWidgetController->CancelPreview();
	
	bPlacing = false;
}

void AThirdMotionPlayerController::MoveCamera(const FInputActionValue& Value)
{
	FVector2D Ax  = Value.Get<FVector2D>();

	APawn* P = GetPawn();
	if (!P) return;

	PRINTLOG(TEXT("Move Camera Called"));

	// 컨트롤 회전 기준 전후좌우
	const FRotator YawRot(0.f, GetControlRotation().Yaw, 0.f);
	const FVector Forward = FRotationMatrix(YawRot).GetUnitAxis(EAxis::X);
	const FVector Right   = FRotationMatrix(YawRot).GetUnitAxis(EAxis::Y);

	P->AddMovementInput(Forward, Ax.Y);
	P->AddMovementInput(Right,   Ax.X);
}

void AThirdMotionPlayerController::MoveCameraUp(const FInputActionValue& Value)
{
	float AxisValue = Value.Get<float>();
	APawn* P = GetPawn();
	if (!P) return;

	FVector Up = FVector::UpVector;
	P->AddMovementInput(Up, AxisValue);
}

void AThirdMotionPlayerController::OnLook(const FInputActionValue& Value)
{
	if (!bLookMode) return;

	PRINTLOG(TEXT("OnLook Called"));

	const FVector2D Ax = Value.Get<FVector2D>(); // X: 마우스X, Y: 마우스Y (프레임당 델타)

	// 마우스는 이미 델타값이므로 보통 DeltaTime을 곱하지 않음
	AddYawInput  ( Ax.X * MouseSensitivity);
	AddPitchInput(-Ax.Y * MouseSensitivity); // 마우스 위로 = 화면 위로면 보통 부호 반전

	// 필요하면 피치 클램프
	FRotator CR = GetControlRotation();
	CR.Pitch = FMath::ClampAngle(CR.Pitch, -85.f, 85.f);
	SetControlRotation(CR);
}

void AThirdMotionPlayerController::OnRMB_Pressed(const FInputActionValue& Value)
{
	PRINTLOG(TEXT("RMB Pressed"));
	bLookMode = true;

	// 커서 숨기고 마우스 캡처
	bShowMouseCursor = false;
	FInputModeGameOnly Mode;
	SetInputMode(Mode);
}

void AThirdMotionPlayerController::OnRMB_Released(const FInputActionValue& Value)
{
	PRINTLOG(TEXT("RMB Released"));
	bLookMode = false;

	// 커서 다시 보이게 (UI도 클릭할거면 GameAndUI로)
	bShowMouseCursor = true;
	FInputModeGameAndUI Mode;
	Mode.SetHideCursorDuringCapture(false);
	SetInputMode(Mode);
}

void AThirdMotionPlayerController::OnClick()
{
	// 프리뷰 고스트가 켜진 상태일 때
	if (bPlacing)
	{
		StopPlacement(true);
		Server_RequestSpawnByTag(CurrentPreset, LastPreviewXf);
	}
	else // 일반 상태일 때
	{
		PRINTLOG(TEXT("SelectUnderCursor"));
		SelectUnderCursor();
	}
}

void AThirdMotionPlayerController::SelectUnderCursor()
{
	FHitResult Hit;
	if (GetHitResultUnderCursorByChannel(UEngineTypes::ConvertToTraceType(ECC_Visibility), true, Hit))
	{
		AActor* NewSel = Hit.GetActor();

		// 기존 하이라이트 끄기
		if (!bGizmoShowed && IsValid(SelectedActor))
			if (auto* H = SelectedActor->FindComponentByClass<UHighlightComponent>())
				H->EnableHighlight(false);

		SelectedActor = NewSel;
		OnActorSelected.Broadcast(SelectedActor);

		// 새 대상 하이라이트 켜기
		if (IsValid(SelectedActor))
		{
			if (auto* H = SelectedActor->FindComponentByClass<UHighlightComponent>())
			{
				H->EnableHighlight(true);
			}
		}

		// SceneController를 통해 선택 전파 (RightPanel(SceneListWidget)에 알림)
		if (MainWidget)
		{
			if (USceneListWidget* SceneListWidget = Cast<USceneListWidget>(MainWidget->RightPanel))
			{
				if (USceneController* SceneController = SceneListWidget->GetSceneController())
				{
					SceneController->SelectActor(SelectedActor);
				}
			}
		}
		
	}
}


void AThirdMotionPlayerController::Server_RequestSpawnByTag_Implementation(FGameplayTag PresetTag, const FTransform& Xf)
{
	if (auto* M = GetWorld()->GetSubsystem<USceneManager>())
		M->SpawnByTag(PresetTag, Xf);
}

void AThirdMotionPlayerController::Server_RequestDestroyByGuid_Implementation(const FGuid& GuidToDestroy)
{
	if (auto* M = GetWorld()->GetSubsystem<USceneManager>())
		M->DestroyByGuid(GuidToDestroy);
}

void AThirdMotionPlayerController::Server_RequestToggleVisibility_Implementation(const FGuid& TargetGuid)
{
	if (!HasAuthority() || !TargetGuid.IsValid()) return;

	if (USceneManager* SceneManager = GetWorld()->GetSubsystem<USceneManager>())
	{
		SceneManager->ToggleActorVisibility(TargetGuid);
	}
}

void AThirdMotionPlayerController::Server_UpdateDirectionalLightRotation_Implementation(FRotator NewRotation)
{
	UE_LOG(LogTemp, Warning, TEXT("[Server RPC] Received rotation update: Pitch=%f"), NewRotation.Pitch);

	// [서버] DirectionalLight 찾기
	TArray<AActor*> FoundLights;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), ADirectionalLight::StaticClass(), FoundLights);

	if (FoundLights.Num() > 0)
	{
		if (ADirectionalLight* Light = Cast<ADirectionalLight>(FoundLights[0]))
		{
			// Mobility 설정
			if (UDirectionalLightComponent* LightComp = Light->FindComponentByClass<UDirectionalLightComponent>())
			{
				if (LightComp->Mobility != EComponentMobility::Movable)
				{
					LightComp->SetMobility(EComponentMobility::Movable);
					UE_LOG(LogTemp, Warning, TEXT("[Server] Set DirectionalLight Mobility to Movable"));
				}
			}

			// 서버에서 회전 적용
			Light->SetActorRotation(NewRotation);
			UE_LOG(LogTemp, Warning, TEXT("[Server] DirectionalLight rotation updated: Pitch=%f"), NewRotation.Pitch);

			// 모든 클라이언트에 동기화 (Multicast)
			Multicast_UpdateDirectionalLightRotation(NewRotation);
		}
		else
		{
			UE_LOG(LogTemp, Error, TEXT("[Server] Failed to cast to DirectionalLight"));
		}
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("[Server] No DirectionalLight found in world!"));
	}
}

void AThirdMotionPlayerController::Multicast_UpdateDirectionalLightRotation_Implementation(FRotator NewRotation)
{
	UE_LOG(LogTemp, Warning, TEXT("[Multicast] Received rotation update: Pitch=%f, IsServer=%d"),
		NewRotation.Pitch, HasAuthority());

	// 모든 클라이언트 + 서버(Listen Server)에서 실행됨
	TArray<AActor*> FoundLights;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), ADirectionalLight::StaticClass(), FoundLights);

	if (FoundLights.Num() > 0)
	{
		if (ADirectionalLight* Light = Cast<ADirectionalLight>(FoundLights[0]))
		{
			// Mobility 설정
			if (UDirectionalLightComponent* LightComp = Light->FindComponentByClass<UDirectionalLightComponent>())
			{
				if (LightComp->Mobility != EComponentMobility::Movable)
				{
					LightComp->SetMobility(EComponentMobility::Movable);
				}
			}

			// 회전 적용
			Light->SetActorRotation(NewRotation);
			UE_LOG(LogTemp, Warning, TEXT("[Multicast] Light rotation applied: Pitch=%f"), NewRotation.Pitch);

			// 로컬 플레이어의 ViewportWidget 슬라이더 업데이트
			if (IsLocalPlayerController())
			{
				// MainWidget → ViewportWidget 찾기
				if (MainWidget && MainWidget->ViewportWidget)
				{
					if (UViewportWidget* ViewportWidget = Cast<UViewportWidget>(MainWidget->ViewportWidget))
					{
						// ReplicatedLightRotation 설정하고 OnRep 수동 호출 (NetActor 패턴)
						ViewportWidget->ReplicatedLightRotation = NewRotation;
						ViewportWidget->OnRep_LightRotation();
						UE_LOG(LogTemp, Warning, TEXT("[Multicast] ViewportWidget updated"));
					}
				}
			}
		}
	}
}

void AThirdMotionPlayerController::Server_UpdateMemoText_Implementation(const FString& NewMemoText)
{
	UE_LOG(LogTemp, Log, TEXT("[Server RPC] Received memo update: %s"), *NewMemoText);

	// 모든 클라이언트에 동기화 (Multicast)
	Multicast_UpdateMemoText(NewMemoText);
}

void AThirdMotionPlayerController::Multicast_UpdateMemoText_Implementation(const FString& NewMemoText)
{
	UE_LOG(LogTemp, Log, TEXT("[Multicast] Received memo update: %s, IsServer=%d"),
		*NewMemoText, HasAuthority());

	// 모든 클라이언트 + 서버(Listen Server)에서 실행됨
	// 로컬 플레이어의 ViewMemo 위젯 업데이트
	if (IsLocalPlayerController())
	{
		// MainWidget 찾기
		TArray<UUserWidget*> FoundWidgets;
		UWidgetBlueprintLibrary::GetAllWidgetsOfClass(GetWorld(), FoundWidgets, UUserWidget::StaticClass());

		for (UUserWidget* Widget : FoundWidgets)
		{
			// WBP_ViewMemo 찾기 (이름으로 확인)
			if (Widget->GetName().Contains(TEXT("ViewMemo")))
			{
				// NoteTextBox 찾기
				if (UMultiLineEditableTextBox* NoteTextBox = Cast<UMultiLineEditableTextBox>(
					Widget->GetWidgetFromName(TEXT("NoteTextBox"))))
				{
					NoteTextBox->SetText(FText::FromString(NewMemoText));
					UE_LOG(LogTemp, Log, TEXT("[Multicast] Updated ViewMemo NoteTextBox"));
					break;
				}
			}
		}
	}
}

void AThirdMotionPlayerController::SetCameraView(ECameraView ViewType)
{
	TArray<AActor*> FoundVolumes;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), APostProcessVolume::StaticClass(), FoundVolumes);

	FVector VolumeCenter = FVector::ZeroVector;
	if (FoundVolumes.Num() > 0)
	{
		VolumeCenter = FoundVolumes[0]->GetActorLocation();
		UE_LOG(LogTemp, Log, TEXT("SetCameraView: PostProcessVolume found at %s"), *VolumeCenter.ToString());
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("SetCameraView: No PostProcessVolume found, using world origin"));
	}

	FRotator NewRotation = FRotator::ZeroRotator;
	FVector NewLocation = FVector::ZeroVector;
	bool bRestoreLocation = false;

	switch (ViewType)
	{
	case ECameraView::Perspective:
		// Perspective 모드: 초기 뷰로 복원
		if (bHasInitialView)
		{
			NewRotation = InitialCameraRotation;
			NewLocation = InitialCameraLocation;
			bRestoreLocation = true;
			UE_LOG(LogTemp, Log, TEXT("Camera View: Perspective (restored to initial view)"));
		}
		else
		{
			NewRotation = GetControlRotation();
			UE_LOG(LogTemp, Log, TEXT("Camera View: Perspective (no initial view saved)"));
		}
		break;

	case ECameraView::Top:
		NewRotation.Pitch = -90.0f;
		NewRotation.Yaw = -90.0f;
		NewRotation.Roll = 0.0f;
		NewLocation = VolumeCenter + FVector(0.0f, 0.0f, Distance);
		bRestoreLocation = true;
		break;

	case ECameraView::Bottom:
		NewRotation.Pitch = 90.0f;
		NewRotation.Yaw = -90.0f;
		NewRotation.Roll = 0.0f;
		NewLocation = VolumeCenter + FVector(0.0f, 0.0f, -Distance);
		bRestoreLocation = true;
		break;

	case ECameraView::Front:
		NewRotation.Pitch = 0.0f;
		NewRotation.Yaw = 0.0f;
		NewRotation.Roll = 0.0f;
		NewLocation = VolumeCenter + FVector(-Distance, 0.0f, 0.0f);
		bRestoreLocation = true;
		break;

	case ECameraView::Back:
		NewRotation.Pitch = 0.0f;
		NewRotation.Yaw = 180.0f;
		NewRotation.Roll = 0.0f;
		NewLocation = VolumeCenter + FVector(Distance, 0.0f,  0.0f);
		bRestoreLocation = true;
		break;

	case ECameraView::Left:
		NewRotation.Pitch = 0.0f;
		NewRotation.Yaw = 90.0f;
		NewRotation.Roll = 0.0f;
		NewLocation = VolumeCenter + FVector(0, -Distance, 0.0f);
		bRestoreLocation = true;
		break;

	case ECameraView::Right:
		NewRotation.Pitch = 0.0f;
		NewRotation.Yaw = -90.0f;
		NewRotation.Roll = 0.0f;
		NewLocation = VolumeCenter + FVector(0, Distance, 0.0f);
		bRestoreLocation = true;
		break;
	}

	// 카메라 회전 적용
	SetControlRotation(NewRotation);

	// Pawn이 있으면 Pawn의 회전 및 위치 설정
	APawn* ControlledPawn = GetPawn();
	if (ControlledPawn)
	{
		ControlledPawn->SetActorRotation(NewRotation);

		// Orthographic 뷰 또는 Perspective 복원 시 위치 설정
		if (bRestoreLocation)
		{
			ControlledPawn->SetActorLocation(NewLocation);
			UE_LOG(LogTemp, Log, TEXT("SetCameraView: Set position - Location=%s, Rotation=%s"),
				*NewLocation.ToString(), *NewRotation.ToString());
		}
		else
		{
			UE_LOG(LogTemp, Log, TEXT("SetCameraView: Applied rotation to Pawn - Pitch=%.2f, Yaw=%.2f, Roll=%.2f"),
				NewRotation.Pitch, NewRotation.Yaw, NewRotation.Roll);
		}
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("SetCameraView: No Pawn found!"));
	}
}

void AThirdMotionPlayerController::Server_RequestApplyTransform_Implementation(const FGuid& Guid,
	const FTransform& NewXf)
{
	if (!HasAuthority()) return;
	if (!Guid.IsValid()) return;
	
	USceneManager* SceneManager = GetWorld()->GetSubsystem<USceneManager>();
	if (!SceneManager) return;
	
	const bool bSuccess = SceneManager->ApplyTransform(Guid, NewXf);
}
