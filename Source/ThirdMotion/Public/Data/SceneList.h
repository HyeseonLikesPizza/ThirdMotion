#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "Data/SceneItemData.h"
#include "SceneList.generated.h"

class AActor;
class USceneItemData;

/**
 * SceneList - DataList 역할
 * SceneItem에서 저장된 데이터를 관리
 */
UCLASS(BlueprintType)
class THIRDMOTION_API USceneList : public UObject
{
	GENERATED_BODY()

public:
	// 데이터 초기화
	void Initialize(UWorld* InWorld);

	// 씬 데이터 갱신
	void RefreshFromWorld();

	// Root 아이템 가져오기
	UFUNCTION(BlueprintCallable, Category = "Scene List")
	TArray<USceneItemData*> GetRootItems() const { return RootItems; }

	// Actor로 ItemData 찾기
	UFUNCTION(BlueprintCallable, Category = "Scene List")
	USceneItemData* FindItemByActor(AActor* Actor) const;

	// 모든 아이템 가져오기 (Flat List)
	UFUNCTION(BlueprintCallable, Category = "Scene List")
	TArray<USceneItemData*> GetAllItems() const;

	// 아이템 개수
	UFUNCTION(BlueprintCallable, Category = "Scene List")
	int32 GetItemCount() const { return ActorToItemMap.Num(); }

	// 델리게이트: 데이터 변경 알림
	DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnDataChanged);
	UPROPERTY(BlueprintAssignable, Category = "Scene List")
	FOnDataChanged OnDataChanged;

private:
	UPROPERTY()
	UWorld* World;

	// Root 아이템들 (부모가 없는 최상위 액터들)
	UPROPERTY()
	TArray<USceneItemData*> RootItems;

	// Actor -> ItemData 매핑
	UPROPERTY()
	TMap<AActor*, USceneItemData*> ActorToItemMap;

	// ItemData 생성
	USceneItemData* CreateItemData(AActor* Actor);

	// 액터 타입 문자열 가져오기
	FString GetActorTypeString(AActor* Actor) const;

	// 액터 필터링 (Actor, Mesh, Light만 포함)
	bool ShouldIncludeActor(AActor* Actor) const;

	// 변경 알림
	void NotifyDataChanged();
};
