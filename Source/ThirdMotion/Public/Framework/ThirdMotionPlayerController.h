#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "GameFramework/PlayerController.h"
#include "Data/ViewportTypes.h"
#include "ThirdMotionPlayerController.generated.h"

struct FInputActionValue;
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

	/* Camera Control */
	void SetCameraView(ECameraView ViewType);

	/* 배치 */
	UFUNCTION(Server, Reliable)
	void Server_RequestSpawnByTag(FGameplayTag PresetTag, const FTransform& Xf);

	/* Actor 속성값 변경 동기화 */
	UFUNCTION(BlueprintCallable, Server, Reliable)
	void Server_RequestApplyTransform(const FGuid& Guid, const FTransform& NewXf);

	UFUNCTION(Server, Reliable)
	void Server_RequestApplyProperty(const FGuid& TargetGuid, const FPropertyDelta& Delta);
	
	/* Actor 삭제 */
	UFUNCTION(Server, Reliable)
	void Server_RequestDestroyByGuid(const FGuid& GuidToDestroy);

	/* Actor 가시성 토글 */
	UFUNCTION(Server, Reliable)
	void Server_RequestToggleVisibility(const FGuid& TargetGuid);

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

	UPROPERTY(EditDefaultsOnly, Category="Input")
	UInputAction* IA_Move;

	UPROPERTY(EditDefaultsOnly, Category="Input")
	UInputAction* IA_MoveUp;

	UPROPERTY(EditDefaultsOnly, Category="Input")
	UInputAction* IA_RMB;
	
	UPROPERTY(EditDefaultsOnly, Category="Input")
	UInputAction* IA_Look;

	UPROPERTY(BlueprintReadWrite, Category="Gizmo")
	bool bGizmoShowed;

	UPROPERTY(EditAnywhere, Category = "Camera")
	float CameraMoveSpeed = 1000.f;

	UPROPERTY(EditAnywhere, Category = "Camera")
	float MouseSensitivity = 1.0f;

	bool bLookMode = false;

	void MoveCamera(const FInputActionValue& Value);
	void MoveCameraUp(const FInputActionValue& Value);
	void OnLook(const FInputActionValue& Value);
	void OnRMB_Pressed(const FInputActionValue& Value);
	void OnRMB_Released(const FInputActionValue& Value);

	/* ----------- Actor Select ----------- */

public:
	AActor* GetSelectedActor();

private:
	// 클릭 이벤트
	void OnClick();

	// 클릭 시 해당 액터 선택
	void SelectUnderCursor();

	UPROPERTY()
	AActor* SelectedActor;

public:
	// 액터 선택 시 델리게이트
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

	// 초기 카메라 위치 및 회전 (Perspective 버튼으로 되돌아갈 뷰)
	FVector InitialCameraLocation;
	FRotator InitialCameraRotation;
	bool bHasInitialView = false;

	float Distance = 500;// PostProcessVolume으로부터 떨어진 거리

};
