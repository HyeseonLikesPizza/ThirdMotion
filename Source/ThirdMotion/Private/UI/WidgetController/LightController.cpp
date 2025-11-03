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
	FPropertyDelta d;
	d.PropertyTag = FGameplayTag::RequestGameplayTag("Property.Light.Intensity");
	d.FloatParam = NewIntensity;

	USceneManager* SceneManager = GetWorld()->GetSubsystem<USceneManager>();

	UEditSyncComponent* ES = LightActor->FindComponentByClass<UEditSyncComponent>();
	if (ES)
	{
		FGuid ActorGuid = ES->GetMeta().Guid;
		SceneManager->ApplyPropertyDelta(ActorGuid, d);
	}
	
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

	OnLightColorChanged.Broadcast(NewColor);
	FPropertyDelta d;
	d.PropertyTag = FGameplayTag::RequestGameplayTag("Property.Light.Color");
	d.ColorParam = NewColor;

	USceneManager* SceneManager = GetWorld()->GetSubsystem<USceneManager>();

	UEditSyncComponent* ES = LightActor->FindComponentByClass<UEditSyncComponent>();
	if (ES)
	{
		FGuid ActorGuid = ES->GetMeta().Guid;
		SceneManager->ApplyPropertyDelta(ActorGuid, d);
	}

	
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

	// EditSyncComponent 가져오기
	UEditSyncComponent* EditSync = LightActor->FindComponentByClass<UEditSyncComponent>();
	if (!EditSync)
	{
		UE_LOG(LogTemp, Warning, TEXT("LightController: EditSyncComponent not found"));
		return;
	}

	// 서버 권한 체크
	bool bIsServer = LightActor->HasAuthority();

	if (bIsServer)
	{
		// 서버인 경우: SceneManager를 통해 직접 적용
		USceneManager* SceneManager = World->GetSubsystem<USceneManager>();
		if (SceneManager)
		{
			const FGuid& ActorGuid = EditSync->GetMeta().Guid;
			if (ActorGuid.IsValid())
			{
				SceneManager->ApplyPropertyDelta(ActorGuid, Delta);
				UE_LOG(LogTemp, Log, TEXT("LightController: Property change applied via SceneManager (Server)"));
			}
		}
	}
	else
	{
		// 클라이언트인 경우: EditSyncComponent RPC 사용
		if (Delta.PropertyTag.GetTagName() == "Property.Light.Intensity")
		{
			EditSync->Server_SetLightIntensity(Delta.FloatParam);
			UE_LOG(LogTemp, Log, TEXT("LightController: Light Intensity RPC called (Client -> Server)"));
		}
		else if (Delta.PropertyTag.GetTagName() == "Property.Light.Color")
		{
			//EditSync->Server_SetLightColor(Delta.ColorParam);
			UE_LOG(LogTemp, Log, TEXT("LightController: Light Color RPC called (Client -> Server)"));
		}
		else
		{
			// 다른 속성은 기존 방식 사용
			UE_LOG(LogTemp, Warning, TEXT("LightController: Client cannot update property %s directly"), *Delta.PropertyTag.ToString());
		}
	}
}
