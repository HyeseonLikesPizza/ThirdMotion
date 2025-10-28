// Fill out your copyright notice in the Description page of Project Settings.

#include "UI/WidgetController/LightController.h"
#include "Edit/SceneManager.h"
#include "Edit/EditSyncComponent.h"
#include "Edit/EditTypes.h"
#include "Edit/LightEditLibrary.h"
#include "Components/LightComponent.h"
#include "GameplayTagContainer.h"

void ULightController::InitializeWithLightActor(AActor* InLightActor)
{
	if (!InLightActor)
	{
		UE_LOG(LogTemp, Warning, TEXT("LightController: Light Actor is null"));
		return;
	}

	LightActor = InLightActor;

	// LightEditLibrary를 통해 현재 값 가져오기
	CurrentIntensity = ULightEditLibrary::GetLightPropertyFloat(InLightActor, ELightPropertyType::Intensity);
	CurrentColor = ULightEditLibrary::GetLightColor(InLightActor);

	UE_LOG(LogTemp, Log, TEXT("LightController: Initialized with Light Actor (Intensity: %f, Color: %s)"),
		CurrentIntensity, *CurrentColor.ToString());
}

void ULightController::SetLightIntensity(float NewIntensity)
{
	if (!LightActor.IsValid())
	{
		UE_LOG(LogTemp, Warning, TEXT("LightController: Light Actor is invalid"));
		return;
	}

	// LightEditLibrary를 통해 Delta 생성
	FPropertyDelta Delta = ULightEditLibrary::MakeLightIntensityDelta(NewIntensity);

	// 서버에 변경 요청
	RequestServerUpdate(Delta);

	// 로컬 상태 업데이트
	CurrentIntensity = NewIntensity;

	// Observer 패턴 - 이벤트 브로드캐스트
	OnLightIntensityChanged.Broadcast(NewIntensity);

	UE_LOG(LogTemp, Log, TEXT("LightController: Intensity changed to %f"), NewIntensity);
}

void ULightController::SetLightColor(FLinearColor NewColor)
{
	if (!LightActor.IsValid())
	{
		UE_LOG(LogTemp, Warning, TEXT("LightController: Light Actor is invalid"));
		return;
	}

	// LightEditLibrary를 통해 Delta 생성
	FPropertyDelta Delta = ULightEditLibrary::MakeLightColorDelta(NewColor);

	// 서버에 변경 요청
	RequestServerUpdate(Delta);

	// 로컬 상태 업데이트
	CurrentColor = NewColor;

	// Observer 패턴 - 이벤트 브로드캐스트
	OnLightColorChanged.Broadcast(NewColor);

	UE_LOG(LogTemp, Log, TEXT("LightController: Color changed to %s"), *NewColor.ToString());
}

void ULightController::RequestServerUpdate(const FPropertyDelta& Delta)
{
	if (!LightActor.IsValid())
		return;

	UWorld* World = LightActor->GetWorld();
	if (!World)
		return;

	// SceneManager 가져오기 (ServerManager 역할)
	USceneManager* SceneManager = World->GetSubsystem<USceneManager>();
	if (!SceneManager)
	{
		UE_LOG(LogTemp, Warning, TEXT("LightController: SceneManager not found"));
		return;
	}

	// 서버 권한 체크
	if (!SceneManager->IsAuthority())
	{
		UE_LOG(LogTemp, Warning, TEXT("LightController: Not authority - cannot update (TODO: Implement client-to-server RPC in GameMode or PlayerController)"));
		return;
	}

	// EditSyncComponent에서 Guid 가져오기
	UEditSyncComponent* EditSync = LightActor->FindComponentByClass<UEditSyncComponent>();
	if (!EditSync)
	{
		UE_LOG(LogTemp, Warning, TEXT("LightController: EditSyncComponent not found"));
		return;
	}

	const FGuid& ActorGuid = EditSync->GetMeta().Guid;
	if (!ActorGuid.IsValid())
	{
		UE_LOG(LogTemp, Warning, TEXT("LightController: Actor Guid is invalid"));
		return;
	}

	// SceneManager를 통해 속성 적용 (ServerManager 역할)
	SceneManager->ApplyPropertyDelta(ActorGuid, Delta);
	UE_LOG(LogTemp, Log, TEXT("LightController: Property change applied via SceneManager"));
}
