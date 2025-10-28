
#pragma once

#include "CoreMinimal.h"
#include "Subsystems/WorldSubsystem.h"
#include "SceneManager.generated.h"

struct FGameplayTag;

UCLASS()
class THIRDMOTION_API USceneManager : public UWorldSubsystem
{
	GENERATED_BODY()

public:
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;
	virtual void Deinitialize() override;
	virtual void OnWorldBeginPlay(UWorld& InWorld) override;

	// 서버만 호출 허용
	bool IsAuthority() const;

	// Guid로 액터 찾기
	AActor* FindByGuid(const FGuid& ID) const;

	// 스폰 / 삭제
	UFUNCTION()
	AActor* SpawnByTag(FGameplayTag PresetTag, const FTransform& T);

	UFUNCTION()
	bool DestroyByGuid(const FGuid& ID);

	// Apply
	UFUNCTION()
	bool ApplyTransform(const FGuid& ID, const FTransform& T);

	UFUNCTION()
	bool ApplyPropertyDelta(const FGuid& Guid, const FPropertyDelta& Delta);

	// Actor 가시성 토글 (네트워크 동기화)
	UFUNCTION()
	bool ToggleActorVisibility(const FGuid& Guid);

private:
	UPROPERTY()
	TMap<FGuid, TWeakObjectPtr<AActor>> GuidCache;

	// SceneList 참조 (자동 업데이트용)
	UPROPERTY()
	TWeakObjectPtr<class USceneList> SceneListRef;

	// Actor 타입별 일련번호 카운터 (네트워크 동기화용)
	UPROPERTY()
	TMap<FString, int32> ActorTypeCounters;

	void AttachEditComponentAndMeta(AActor* Actor, const struct FLibraryRow& Row);

	// 고유 Actor 이름 생성 (중앙 집중식)
	FString GenerateUniqueActorName(const FString& BaseClassName);

public:
	// SceneList 등록 (RightPanel에서 호출)
	void RegisterSceneList(class USceneList* InSceneList);

	// SceneList 접근자 (LibraryWidgetController에서 사용)
	class USceneList* GetSceneList() const { return SceneListRef.Get(); }

private:

	// 델리게이트 핸들
	FDelegateHandle OnSpawnedHandle;
	FDelegateHandle OnDestroyedHandle;

	// 준비
	void OnResolverReady();
	void BuildInitialGuidCache();
	void BindWorldDelegates();
	void UnbindWorldDelegates();

	// 델리게이트 콜백
	void HandleActorSpawned(AActor* NewActor);
	void HandleActorDestroyed(AActor* DestroyedActor);
	
};
