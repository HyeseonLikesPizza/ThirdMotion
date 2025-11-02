#include "UI/WidgetController/SceneController.h"
#include "GameFramework/Actor.h"
#include "Engine/World.h"
#include "Edit/SceneManager.h"
#include "Edit/EditSyncComponent.h"
#include "Framework/ThirdMotionPlayerController.h"
#include "Edit/EditTypes.h"

void USceneController::Initialize(UWorld* InWorld)
{
	World = InWorld;
	SelectedActors.Empty();
}

void USceneController::SelectActor(AActor* Actor)
{
	if (!Actor) return;

	SelectedActors.Empty();
	SelectedActors.Add(Actor);

	NotifySelectionChanged();

}

void USceneController::SelectActors(const TArray<AActor*>& Actors)
{
	SelectedActors = Actors;
	NotifySelectionChanged();

}

void USceneController::ClearSelection()
{
	SelectedActors.Empty();
	NotifySelectionChanged();

}

TArray<AActor*> USceneController::GetSelectedActors() const
{
	return SelectedActors;
}

// 가시성 제어
void USceneController::ToggleActorVisibility(AActor* Actor)
{
	if (!Actor || !World) return;

	// EditSyncComponent가 있으면 서버 RPC로 동기화
	if (UEditSyncComponent* EditComp = Actor->FindComponentByClass<UEditSyncComponent>())
	{
		const FGuid ActorGuid = EditComp->GetMeta().Guid;
		if (ActorGuid.IsValid())
		{
			UE_LOG(LogTemp, Log, TEXT("SceneController: Toggling visibility via RPC - GUID=%s"),
				*ActorGuid.ToString());

			// PlayerController를 통해 Server RPC 호출
			if (APlayerController* PC = World->GetFirstPlayerController())
			{
				if (AThirdMotionPlayerController* TMPC = Cast<AThirdMotionPlayerController>(PC))
				{
					TMPC->Server_RequestToggleVisibility(ActorGuid);
				}
			}
		}
		else
		{
			// GUID가 없으면 로컬에서만 처리
			UE_LOG(LogTemp, Warning, TEXT("SceneController: Actor has no GUID, toggling locally"));
			bool bCurrentlyHidden = Actor->IsHidden();
			bool bNewVisibility = !bCurrentlyHidden; // 현재 숨겨져 있으면 보이게, 보이면 숨기기
			SetActorVisibility(Actor, bNewVisibility);
		}
	}
	else
	{
		// EditSyncComponent가 없으면 로컬에서만 처리 (Ghost 등)
		UE_LOG(LogTemp, Warning, TEXT("SceneController: Actor has no EditSyncComponent, toggling locally"));
		bool bCurrentlyHidden = Actor->IsHidden();
		bool bNewVisibility = !bCurrentlyHidden; // 현재 숨겨져 있으면 보이게, 보이면 숨기기
		SetActorVisibility(Actor, bNewVisibility);
	}
}

void USceneController::SetActorVisibility(AActor* Actor, bool bVisible)
{
	if (!Actor) return;

	// 로컬 가시성 설정
	Actor->SetActorHiddenInGame(!bVisible);

#if WITH_EDITOR
	Actor->SetIsTemporarilyHiddenInEditor(!bVisible);
#endif

	// 씬 변경 알림 (SceneListWidget이 자동으로 UI 업데이트)
	NotifySceneChanged();

	UE_LOG(LogTemp, Log, TEXT("SceneController: Actor visibility set - Actor: %s, Visible: %d"),
		*Actor->GetName(), bVisible);
}

// 이름 변경
void USceneController::RenameActor(AActor* Actor, const FString& NewName)
{
	/*if (!Actor || NewName.IsEmpty()) return;

	Actor->SetActorLabel(NewName);

	NotifySceneChanged();*/

}

// 액터 삭제
void USceneController::DeleteActor(AActor* Actor)
{
	if (!Actor || !World) return;

	// EditSyncComponent가 있으면 SceneManager를 통해 삭제 (네트워크 동기화)
	if (UEditSyncComponent* EditComp = Actor->FindComponentByClass<UEditSyncComponent>())
	{
		const FGuid ActorGuid = EditComp->GetMeta().Guid;
		if (ActorGuid.IsValid())
		{
			UE_LOG(LogTemp, Warning, TEXT("SceneController: Deleting actor via RPC - GUID=%s"),
				*ActorGuid.ToString());

			// PlayerController를 통해 Server RPC 호출
			if (APlayerController* PC = World->GetFirstPlayerController())
			{
				if (AThirdMotionPlayerController* TMPC = Cast<AThirdMotionPlayerController>(PC))
				{
					TMPC->Server_RequestDestroyByGuid(ActorGuid);
				}
			}
		}
		else
		{
			// GUID가 없으면 직접 삭제
			UE_LOG(LogTemp, Warning, TEXT("SceneController: Actor has no GUID, deleting directly"));
			Actor->Destroy();
		}
	}
	else
	{
		// EditSyncComponent가 없으면 직접 삭제 (Ghost 등)
		UE_LOG(LogTemp, Warning, TEXT("SceneController: Actor has no EditSyncComponent, deleting directly"));
		Actor->Destroy();
	}

	// 선택된 목록에서 제거
	SelectedActors.Remove(Actor);

	NotifySelectionChanged();
	NotifySceneChanged();
}

void USceneController::DeleteSelectedActors()
{
	TArray<AActor*> ActorsToDelete = SelectedActors;

	for (AActor* Actor : ActorsToDelete)
	{
		if (Actor)
		{
			Actor->Destroy();
		}
	}

	SelectedActors.Empty();

	NotifySelectionChanged();
	NotifySceneChanged();

}

// 액터 복제
AActor* USceneController::DuplicateActor(AActor* Actor)
{
	if (!Actor || !World) return nullptr;

	// 간단한 복제 (Transform만 복사)
	FActorSpawnParameters SpawnParams;
	SpawnParams.Template = Actor;

	AActor* NewActor = World->SpawnActor<AActor>(
		Actor->GetClass(),
		Actor->GetActorTransform(),
		SpawnParams
	);

	if (NewActor)
	{
		FString NewName = Actor->GetActorLabel() + TEXT("_Copy");
		NewActor->SetActorLabel(NewName);

		NotifySceneChanged();

	}

	return NewActor;
}

// 알림 함수
void USceneController::NotifySelectionChanged()
{
	OnSelectionChanged.Broadcast(SelectedActors);
}

void USceneController::NotifySceneChanged()
{
	OnSceneChanged.Broadcast();
}
