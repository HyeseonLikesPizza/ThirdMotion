#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "GameFramework/PlayerController.h"
#include "ThirdMotionPlayerController.generated.h"

class ULibraryWidgetController;
struct FGameplayTag;
class UUserWidget;
class UMainWidget;
class UInputMappingContext;
class UInputAction;

UCLASS()
class THIRDMOTION_API AThirdMotionPlayerController : public APlayerController
{
	GENERATED_BODY()

public:
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaSeconds) override;
	virtual void SetupInputComponent() override;

	/* 배치 */
	UFUNCTION(Server, Reliable)
	void Server_RequestSpawnByTag(FGameplayTag PresetTag, const FTransform& Xf);
	
	void StartPlacement(const FGameplayTag& PresetTag);
	void StopPlacement(bool bCancel);

	// 메인 오버레이 클래스
	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<UUserWidget> MainWidgetClass;

	/* Enhanced Input */
	
	UPROPERTY(EditDefaultsOnly, Category="Input")
	UInputMappingContext* IMC;

	UPROPERTY(EditDefaultsOnly, Category="Input")
	UInputAction* IA_Click;

	UPROPERTY(BlueprintReadWrite, Category="Gizmo")
	bool bGizmoShowed;

	
private:
	
	void OnClick();

	/* Actor Select */

	UPROPERTY()
	AActor* SelectedActor;
	
	void SelectUnderCursor();


	

	/* 메인 오버레이 위젯 */
	
	UPROPERTY()
	UMainWidget* MainWidget;

	UPROPERTY()
	ULibraryWidgetController* LibraryWidgetController;

	// 미리보기 활성화 상태
	bool bPlacing = false;
	FGameplayTag CurrentPreset;
	FTransform LastPreviewXf;
	
};
