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
#include "UI/WidgetController/LibraryWidgetController.h"

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

	// 메인 오버레이 생성
	
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
		EIC->BindAction(IA_Click, ETriggerEvent::Triggered, this, &AThirdMotionPlayerController::OnClick);
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
		Server_RequestSpawnByTag(CurrentPreset, LastPreviewXf);
	} // 일반 상태일 때
	else
	{
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
		if (IsValid(SelectedActor))
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
	}
}


void AThirdMotionPlayerController::Server_RequestSpawnByTag_Implementation(FGameplayTag PresetTag, const FTransform& Xf)
{
	if (auto* M = GetWorld()->GetSubsystem<USceneManager>())
		M->SpawnByTag(PresetTag, Xf);
}
