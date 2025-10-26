#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "ServerController.generated.h"

class AActor;
class USceneList;

/**
 * ServerController - 네트워크 동기화 중앙 관리 (GameInstance Subsystem)
 *
 * 책임:
 * - Scene 관련 네트워크 동기화 (Actor 추가/삭제/수정)
 * - Camera 관련 네트워크 동기화
 * - OnRep_Meta 처리 (클라이언트에서 복제 완료 시)
 * - World별 SceneList 업데이트 관리
 */
UCLASS()
class THIRDMOTION_API UServerController : public UGameInstanceSubsystem
{
	GENERATED_BODY()

public:
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;
	virtual void Deinitialize() override;

	// World별 SceneList 등록 (클라이언트/서버 모두)
	void RegisterSceneList(UWorld* World, USceneList* InSceneList);

	// OnRep_Meta 처리 (클라이언트에서 DisplayName 복제 완료 시)
	void HandleMetaReplicated(AActor* Actor);

	// Scene 업데이트 알림
	void NotifySceneChanged(UWorld* World);

private:
	// World별 SceneList 참조 맵 (멀티 World 지원)
	UPROPERTY()
	TMap<UWorld*, TWeakObjectPtr<USceneList>> SceneListMap;
};
