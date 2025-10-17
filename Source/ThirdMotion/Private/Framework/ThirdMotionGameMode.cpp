
#include "Framework/ThirdMotionGameMode.h"
#include "Framework/ThirdMotionPlayerController.h"
#include "Blueprint/UserWidget.h"

AThirdMotionGameMode::AThirdMotionGameMode()
{
	PlayerControllerClass = AThirdMotionPlayerController::StaticClass();
	
}

void AThirdMotionGameMode::BeginPlay()
{
	Super::BeginPlay();
}
