#include "Framework/ThirdMotionPlayerController.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "GameplayTagContainer.h"
#include "Edit/SceneManager.h"
#include "Blueprint/UserWidget.h"
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
#include "Kismet/GameplayStatics.h"
#include "UI/Widget/ViewportWidget.h"

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
		if (bGizmoShowed) return;
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

		// 새 대상 하이라이트 켜기
		if (IsValid(SelectedActor))
		{
			if (auto* H = SelectedActor->FindComponentByClass<UHighlightComponent>())
			{
				H->EnableHighlight(true);
			}
		}

		// SceneController를 통해 선택 전파 (RightPanel에 알림)
		if (MainWidget)
		{
			if (URightPanel* RightPanel = Cast<URightPanel>(MainWidget->RightPanel))
			{
				if (USceneController* SceneController = RightPanel->GetSceneController())
				{
					SceneController->SelectActor(SelectedActor);
				}
			}
		}

		// 
	}
}


void AThirdMotionPlayerController::Server_RequestSpawnByTag_Implementation(FGameplayTag PresetTag, const FTransform& Xf)
{
	if (auto* M = GetWorld()->GetSubsystem<USceneManager>())
		M->SpawnByTag(PresetTag, Xf);
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
