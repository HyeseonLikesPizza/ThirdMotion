
#include "Framework/ThirdMotionPlayerController.h"

#include "GameplayTagContainer.h"
#include "Edit/SceneManager.h"
#include "Blueprint/UserWidget.h"
#include "TimerManager.h"
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

	
}


void AThirdMotionPlayerController::Server_RequestSpawnByTag_Implementation(FGameplayTag PresetTag, const FTransform& Xf)
{
	if (auto* M = GetWorld()->GetSubsystem<USceneManager>())
		M->SpawnByTag(PresetTag, Xf);
}
