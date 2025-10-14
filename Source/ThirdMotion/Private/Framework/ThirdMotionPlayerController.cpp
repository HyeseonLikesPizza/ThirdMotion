
#include "Framework/ThirdMotionPlayerController.h"

#include "GameplayTagContainer.h"
#include "Edit/SceneManager.h"

void AThirdMotionPlayerController::Server_RequestSpawnByTag_Implementation(FGameplayTag PresetTag, const FTransform& Xf)
{
	if (auto* M = GetWorld()->GetSubsystem<USceneManager>())
		M->SpawnByTag(PresetTag, Xf);
}
