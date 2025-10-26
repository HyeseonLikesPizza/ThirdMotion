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

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnActorSelected, AActor*, SelectedActor);

UCLASS()
class THIRDMOTION_API AThirdMotionPlayerController : public APlayerController
{
	GENERATED_BODY()

public:
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaSeconds) override;
	virtual void SetupInputComponent() override;

	/* */
	AActor* GetSelectedActor();

	/* 배치 */
	UFUNCTION(Server, Reliable)
	void Server_RequestSpawnByTag(FGameplayTag PresetTag, const FTransform& Xf);

	void StartPlacement(const FGameplayTag& PresetTag);
	void StopPlacement(bool bCancel);

	/* DirectionalLight 동기화 */
	UFUNCTION(Server, Reliable)
	void Server_UpdateDirectionalLightRotation(FRotator NewRotation);

	UFUNCTION(NetMulticast, Reliable)
	void Multicast_UpdateDirectionalLightRotation(FRotator NewRotation);

	// 메인 오버레이 클래스
	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<UUserWidget> MainWidgetClass;

	// 로딩 화면 위젯 클래스
	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<UUserWidget> LoadingWidgetClass;

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

public:
	FOnActorSelected OnActorSelected;

private:

	/* 메인 오버레이 위젯 */

	UPROPERTY()
	UMainWidget* MainWidget;

	UPROPERTY()
	UUserWidget* LoadingWidget;

	UPROPERTY()
	ULibraryWidgetController* LibraryWidgetController;

	// 로딩 완료 후 메인 위젯 표시
	void ShowMainWidget();

	// 미리보기 활성화 상태
	bool bPlacing = false;
	FGameplayTag CurrentPreset;
	FTransform LastPreviewXf;
	
};
