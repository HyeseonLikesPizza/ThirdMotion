#include "Network/ServerController.h"
#include "Engine/World.h"
#include "Data/SceneList.h"
#include "Edit/EditSyncComponent.h"
#include "Edit/EditTypes.h"
#include "GameplayTagContainer.h"

void UServerController::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);

	UE_LOG(LogTemp, Log, TEXT("ServerController: GameInstance Subsystem Initialized"));
}

void UServerController::Deinitialize()
{
	SceneListMap.Empty();

	Super::Deinitialize();

	UE_LOG(LogTemp, Log, TEXT("ServerController: GameInstance Subsystem Deinitialized"));
}

void UServerController::RegisterSceneList(UWorld* World, USceneList* InSceneList)
{
	if (!World || !InSceneList)
	{
		UE_LOG(LogTemp, Warning, TEXT("ServerController: Cannot register null World or SceneList"));
		return;
	}

	SceneListMap.Add(World, InSceneList);

	UE_LOG(LogTemp, Log, TEXT("ServerController: SceneList registered - World=%s, NetMode=%d"),
		*World->GetName(), World->GetNetMode());
}

void UServerController::HandleMetaReplicated(AActor* Actor)
{
	if (!Actor)
	{
		UE_LOG(LogTemp, Warning, TEXT("ServerController: HandleMetaReplicated - Invalid Actor"));
		return;
	}

	UWorld* World = Actor->GetWorld();
	if (!World)
	{
		UE_LOG(LogTemp, Warning, TEXT("ServerController: HandleMetaReplicated - Actor has no World"));
		return;
	}

	// 클라이언트에서만 처리 (서버는 이미 HandleActorSpawned에서 처리됨)
	if (World->GetNetMode() == NM_Client)
	{
		TWeakObjectPtr<USceneList>* SceneListPtr = SceneListMap.Find(World);
		if (SceneListPtr && SceneListPtr->IsValid())
		{
			USceneList* SceneList = SceneListPtr->Get();

			UE_LOG(LogTemp, Log, TEXT("ServerController: HandleMetaReplicated - Updating SceneList for actor %s"),
				*Actor->GetName());

			// 기존 항목 제거 후 재추가 (DisplayName 업데이트)
			SceneList->RemoveActor(Actor);
			SceneList->AddActor(Actor);
		}
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("ServerController: HandleMetaReplicated - SceneList not registered for World %s"),
				*World->GetName());
		}
	}
}

void UServerController::NotifySceneChanged(UWorld* World)
{
	if (!World)
	{
		UE_LOG(LogTemp, Warning, TEXT("ServerController: NotifySceneChanged - Invalid World"));
		return;
	}

	TWeakObjectPtr<USceneList>* SceneListPtr = SceneListMap.Find(World);
	if (SceneListPtr && SceneListPtr->IsValid())
	{
		// SceneList의 OnDataChanged 델리게이트 브로드캐스트
		// (이미 AddActor/RemoveActor에서 호출되지만 수동 알림용)
		UE_LOG(LogTemp, Log, TEXT("ServerController: NotifySceneChanged for World %s"), *World->GetName());
	}
}

FGameplayTag UServerController::GetActorPresetTag(AActor* Actor) const
{
	if (!Actor)
	{
		UE_LOG(LogTemp, Warning, TEXT("ServerController: GetActorPresetTag - Invalid Actor"));
		return FGameplayTag::EmptyTag;
	}

	// EditSyncComponent에서 PresetTag 가져오기
	UEditSyncComponent* EditComp = Actor->FindComponentByClass<UEditSyncComponent>();
	if (EditComp)
	{
		const FEditMeta& Meta = EditComp->GetMeta();
		return Meta.PresetTag;
	}

	UE_LOG(LogTemp, Warning, TEXT("ServerController: GetActorPresetTag - Actor has no EditSyncComponent"));
	return FGameplayTag::EmptyTag;
}
