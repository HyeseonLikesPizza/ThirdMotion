
#include "Framework/ThirdMotionPlayerController.h"

#include "GameplayTagContainer.h"
#include "Edit/SceneManager.h"
#include "Blueprint/UserWidget.h"
#include "TimerManager.h"
#include "Engine/World.h"

void AThirdMotionPlayerController::BeginPlay()
{
	Super::BeginPlay();

}


void AThirdMotionPlayerController::Server_RequestSpawnByTag_Implementation(FGameplayTag PresetTag, const FTransform& Xf)
{
	if (auto* M = GetWorld()->GetSubsystem<USceneManager>())
		M->SpawnByTag(PresetTag, Xf);
}
