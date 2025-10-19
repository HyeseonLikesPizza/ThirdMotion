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
	ItemData->DisplayName = Actor->GetActorLabel();
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

void USceneList::NotifyDataChanged()
{
	OnDataChanged.Broadcast();
}
