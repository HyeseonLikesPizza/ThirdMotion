
#pragma once

#include "CoreMinimal.h"
#include "ViewportTypes.generated.h"

UENUM(BlueprintType)
enum class ECameraView : uint8
{
	Perspective,
	Top,
	Bottom,
	Left,
	Right,
	Front,
	Back
};
