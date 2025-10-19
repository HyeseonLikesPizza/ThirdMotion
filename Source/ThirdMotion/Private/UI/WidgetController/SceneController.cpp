#include "UI/WidgetController/SceneController.h"
#include "GameFramework/Actor.h"
#include "Engine/World.h"

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
	if (!Actor) return;

	bool bNewVisibility = Actor->IsHidden();
	SetActorVisibility(Actor, bNewVisibility);
}

void USceneController::SetActorVisibility(AActor* Actor, bool bVisible)
{
	if (!Actor) return;

	// 액터 가시성 설정
	Actor->SetActorHiddenInGame(!bVisible);

#if WITH_EDITOR
	Actor->SetIsTemporarilyHiddenInEditor(!bVisible);
#endif

	NotifySceneChanged();

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
	/*if (!Actor) return;

	FString ActorName = Actor->GetActorLabel();

	// 선택된 목록에서 제거
	SelectedActors.Remove(Actor);

	// 액터 삭제
	Actor->Destroy();

	NotifySelectionChanged();
	NotifySceneChanged();*/

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
