
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


private:
	UPROPERTY()
	TMap<FGuid, TWeakObjectPtr<AActor>> GuidCache;

	void AttachEditComponentAndMeta(AActor* Actor, const struct FLibraryRow& Row);

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
