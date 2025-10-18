
#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "SceneTreeItem.generated.h"

UCLASS(BlueprintType)
class THIRDMOTION_API USceneRowObject : public UObject
{
	GENERATED_BODY()
public:
	// 표시용
	UPROPERTY(BlueprintReadOnly) FText DisplayName;
	// 실제 대상
	UPROPERTY(BlueprintReadOnly) TWeakObjectPtr<AActor> Actor;
	// 들여쓰기용(계층)
	UPROPERTY(BlueprintReadOnly) int32 Depth = 0;

	void Init(AActor* InActor, int32 InDepth)
	{
		Actor = InActor;
#if WITH_EDITOR
		// 에디터에서 보이는 라벨
		const FString Label = InActor ? InActor->GetActorLabel() : TEXT("");
		DisplayName = FText::FromString(Label);
#else
		DisplayName = FText::FromString(InActor ? InActor->GetName() : TEXT(""));
#endif
		Depth = InDepth;
	}
};
