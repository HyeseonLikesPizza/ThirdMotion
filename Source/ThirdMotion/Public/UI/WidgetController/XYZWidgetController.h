
#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "XYZWidgetController.generated.h"

class UXYZWidget;

UCLASS()
class THIRDMOTION_API UXYZWidgetController : public UObject
{
	GENERATED_BODY()

public:
	void Initialize(class UUserWidget* InView);
	void SetTargetActor(AActor* InActor);

	// 위젯에서 호출
	void OnPosCommitted(float X, float Y, float Z);
	void OnRotCommitted(float Pitch, float Yaw, float Roll);
	void OnScaleCommitted(float SXpct, float SYpct, float SZpct);
	void OnSizeCommitted(float SXm, float SYm, float SZm);

	// 주기 싱크
	void TickSync();

private:
	TWeakObjectPtr<AActor> Target;
	TWeakObjectPtr<UXYZWidget> View;

	FVector GetWorldSize(AActor* A) const;     // cm
	void   PushTransform(const FTransform& NewWorldTx);
};
