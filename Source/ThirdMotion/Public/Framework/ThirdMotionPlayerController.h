
#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "ThirdMotionPlayerController.generated.h"

struct FGameplayTag;

UCLASS()
class THIRDMOTION_API AThirdMotionPlayerController : public APlayerController
{
	GENERATED_BODY()

public:
	UFUNCTION(Server, Reliable)
	void Server_RequestSpawnByTag(FGameplayTag PresetTag, const FTransform& Xf);
};
