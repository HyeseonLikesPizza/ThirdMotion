
#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameMode.h"
#include "ThirdMotionGameMode.generated.h"


UCLASS()
class THIRDMOTION_API AThirdMotionGameMode : public AGameMode
{
	GENERATED_BODY()

public:
	AThirdMotionGameMode();

	virtual void BeginPlay() override;
};
