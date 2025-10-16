
#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "ThirdMotionPlayerController.generated.h"

struct FGameplayTag;
class UUserWidget;

UCLASS()
class THIRDMOTION_API AThirdMotionPlayerController : public APlayerController
{
	GENERATED_BODY()

public:
	virtual void BeginPlay() override;

	UFUNCTION(Server, Reliable)
	void Server_RequestSpawnByTag(FGameplayTag PresetTag, const FTransform& Xf);


};
