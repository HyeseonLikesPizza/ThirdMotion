#include "Data/SceneList.h"
#include "Data/SceneItemData.h"
#include "GameFramework/Actor.h"
#include "Engine/World.h"
#include "EngineUtils.h"
#include "Engine/Light.h"
#include "Engine/DirectionalLight.h"
#include "Engine/PointLight.h"
#include "Engine/SpotLight.h"
#include "Camera/CameraActor.h"
#include "Engine/StaticMeshActor.h"
#include "Edit/EditSyncComponent.h"

void USceneList::Initialize(UWorld* InWorld)
{
	World = InWorld;
	RefreshFromWorld();
}

void USceneList::RefreshFromWorld()
{
	if (!World) return;

	// 기존 데이터 클리어
	RootItems.Empty();
	ActorToItemMap.Empty();

	// 월드의 모든 액터 가져오기
	for (TActorIterator<AActor> It(World); It; ++It)
	{
		AActor* Actor = *It;
		if (!Actor) continue;

		// Actor, Mesh, Light만 필터링
		if (!ShouldIncludeActor(Actor))
		{
			UE_LOG(LogTemp, Warning, TEXT("SceneList: Filtered out - %s (%s)"),
				*Actor->GetActorLabel(), *Actor->GetClass()->GetName());
			continue;
		}

		UE_LOG(LogTemp, Log, TEXT("SceneList: Added - %s (%s)"),
			*Actor->GetActorLabel(), *Actor->GetClass()->GetName());

		// ItemData 생성
		USceneItemData* ItemData = CreateItemData(Actor);
		ActorToItemMap.Add(Actor, ItemData);

		// 부모가 없으면 루트 아이템
		if (!Actor->GetAttachParentActor())
		{
			RootItems.Add(ItemData);
		}
	}

	// 자식 관계 설정
	for (auto& Pair : ActorToItemMap)
	{
		AActor* Actor = Pair.Key;
		USceneItemData* ItemData = Pair.Value;

		if (AActor* ParentActor = Actor->GetAttachParentActor())
		{
			if (USceneItemData** ParentDataPtr = ActorToItemMap.Find(ParentActor))
			{
				(*ParentDataPtr)->Children.Add(ItemData);
			}
		}
	}

	NotifyDataChanged();

}

void USceneList::UpdateAllItems()
{
	// 기존 ItemData들의 Actor 상태 업데이트 (가시성 등)
	for (auto& Pair : ActorToItemMap)
	{
		USceneItemData* ItemData = Pair.Value;
		if (ItemData && ItemData->Actor)
		{
			ItemData->UpdateFromActor();
		}
	}
}

USceneItemData* USceneList::FindItemByActor(AActor* Actor) const
{
	if (USceneItemData* const* ItemPtr = ActorToItemMap.Find(Actor))
	{
		return *ItemPtr;
	}
	return nullptr;
}

TArray<USceneItemData*> USceneList::GetAllItems() const
{
	TArray<USceneItemData*> AllItems;
	ActorToItemMap.GenerateValueArray(AllItems);
	return AllItems;
}

USceneItemData* USceneList::CreateItemData(AActor* Actor)
{
	USceneItemData* ItemData = NewObject<USceneItemData>(this);
	ItemData->Actor = Actor;

	// EditSyncComponent가 있으면 복제된 DisplayName 사용
	if (UEditSyncComponent* EditComp = Actor->FindComponentByClass<UEditSyncComponent>())
	{
		const FEditMeta& Meta = EditComp->GetMeta();
		if (!Meta.DisplayName.IsNone())
		{
			ItemData->DisplayName = Meta.DisplayName.ToString();
		}
		else
		{
			ItemData->DisplayName = Actor->GetActorLabel();
		}
	}
	else
	{
		// EditSyncComponent가 없으면 ActorLabel 사용 (Ghost 등)
		ItemData->DisplayName = Actor->GetActorLabel();
	}

	ItemData->ActorType = GetActorTypeString(Actor);
	ItemData->bIsVisible = !Actor->IsHidden();
	ItemData->bIsExpanded = false;

	return ItemData;
}

FString USceneList::GetActorTypeString(AActor* Actor) const
{
	if (!Actor) return TEXT("Unknown");

	if (Actor->IsA<ACameraActor>())
	{
		return TEXT("Camera");
	}
	else if (Actor->IsA<ADirectionalLight>())
	{
		return TEXT("Directional Light");
	}
	else if (Actor->IsA<APointLight>())
	{
		return TEXT("Point Light");
	}
	else if (Actor->IsA<ASpotLight>())
	{
		return TEXT("Spot Light");
	}
	else if (Actor->IsA<ALight>())
	{
		return TEXT("Light");
	}
	else if (Actor->IsA<AStaticMeshActor>())
	{
		return TEXT("Static Mesh");
	}
	else
	{
		// 클래스 이름에서 'A' 접두사 제거
		FString ClassName = Actor->GetClass()->GetName();
		if (ClassName.StartsWith(TEXT("A")))
		{
			ClassName.RightChopInline(1);
		}
		return ClassName;
	}
}

bool USceneList::ShouldIncludeActor(AActor* Actor) const
{
	if (!Actor) return false;

	FString ClassName = Actor->GetClass()->GetName();

	// Light 클래스만 포함 (DirectionalLight, PointLight, SpotLight, SkyLight 등)
	if (Actor->IsA<ALight>())
	{
		return true;
	}

	// StaticMeshActor만 포함
	if (Actor->IsA<AStaticMeshActor>())
	{
		return true;
	}

	// 커스텀 Blueprint Actor만 포함 (BP_로 시작하고 _C로 끝나는 것)
	// 예: BP_Chair1_C, BP_Pawn_C 등
	if (ClassName.StartsWith(TEXT("BP_")) && ClassName.EndsWith(TEXT("_C")))
	{
		// 시스템 BP는 제외 (GameMode, PlayerController 등)
		if (ClassName.Contains(TEXT("GameMode")) ||
			ClassName.Contains(TEXT("PlayerController")) ||
			ClassName.Contains(TEXT("Pawn")))
		{
			return false;
		}
		return true;
	}

	// 나머지는 모두 제외
	return false;
}

void USceneList::AddActor(AActor* Actor)
{
	if (!Actor)
	{
		UE_LOG(LogTemp, Error, TEXT("SceneList: AddActor - Actor is NULL"));
		return;
	}

	if (!ShouldIncludeActor(Actor))
	{
		UE_LOG(LogTemp, Warning, TEXT("SceneList: AddActor - Actor filtered out: %s (%s)"),
			*Actor->GetActorLabel(), *Actor->GetClass()->GetName());
		return;
	}

	// 이미 존재하면 무시
	if (ActorToItemMap.Contains(Actor))
	{
		UE_LOG(LogTemp, Warning, TEXT("SceneList: AddActor - Actor already exists: %s"),
			*Actor->GetActorLabel());
		return;
	}

	// ItemData 생성
	USceneItemData* ItemData = CreateItemData(Actor);
	ActorToItemMap.Add(Actor, ItemData);

	UE_LOG(LogTemp, Warning, TEXT("SceneList: AddActor - Added: %s (DisplayName=%s)"),
		*Actor->GetActorLabel(), *ItemData->DisplayName);

	// 부모 관계 설정
	AActor* ParentActor = Actor->GetAttachParentActor();
	if (ParentActor)
	{
		if (USceneItemData** ParentDataPtr = ActorToItemMap.Find(ParentActor))
		{
			(*ParentDataPtr)->Children.Add(ItemData);
		}
	}
	else
	{
		// 부모가 없으면 루트 아이템
		RootItems.Add(ItemData);
	}

	NotifyDataChanged();
}

void USceneList::RemoveActor(AActor* Actor)
{
	if (!Actor) return;

	USceneItemData** ItemDataPtr = ActorToItemMap.Find(Actor);
	if (!ItemDataPtr) return;

	USceneItemData* ItemData = *ItemDataPtr;

	// 부모에서 제거
	AActor* ParentActor = Actor->GetAttachParentActor();
	if (ParentActor)
	{
		if (USceneItemData** ParentDataPtr = ActorToItemMap.Find(ParentActor))
		{
			(*ParentDataPtr)->Children.Remove(ItemData);
		}
	}
	else
	{
		// 루트 아이템에서 제거
		RootItems.Remove(ItemData);
	}

	// 맵에서 제거
	ActorToItemMap.Remove(Actor);

	NotifyDataChanged();
}

void USceneList::NotifyDataChanged()
{
	OnDataChanged.Broadcast();
}
