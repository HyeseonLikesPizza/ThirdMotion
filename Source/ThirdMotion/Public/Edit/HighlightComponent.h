
#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "HighlightComponent.generated.h"


UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class THIRDMOTION_API UHighlightComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UHighlightComponent();
	void EnableHighlight(bool bEnable);

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Highlight", meta=(ClampMin="0", ClampMax="255"))
	int32 StencilValue = 1;

};
