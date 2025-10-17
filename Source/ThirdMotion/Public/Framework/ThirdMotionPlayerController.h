
#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "ThirdMotionPlayerController.generated.h"

class ULibraryWidgetController;
struct FGameplayTag;
class UUserWidget;
class UMainWidget;

UCLASS()
class THIRDMOTION_API AThirdMotionPlayerController : public APlayerController
{
	GENERATED_BODY()

public:
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaSeconds) override;

	UFUNCTION(Server, Reliable)
	void Server_RequestSpawnByTag(FGameplayTag PresetTag, const FTransform& Xf);

	// 메인 오버레이 클래스
	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<UUserWidget> MainWidgetClass;

private:

	// 메인 오버레이 위젯
	UPROPERTY()
	UMainWidget* MainWidget;

	UPROPERTY()
	ULibraryWidgetController* LibraryWidgetController;

	
};
