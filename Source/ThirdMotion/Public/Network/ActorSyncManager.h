// 향후 NetworkManager가 필요할 경우를 대비한 예시
// 현재는 사용하지 않음 - PlayerController에서 직접 처리

#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "ActorSyncManager.generated.h"

/**
 * ActorSyncManager (선택적 구현)
 *
 * 사용 시기:
 * - Mesh/Material 외에 Transform, Physics, Custom Properties 등 동기화 항목이 10개 이상
 * - 여러 Controller가 동일한 동기화 로직을 공유해야 할 때
 * - Dedicated Server에서 PlayerController 없이 동기화가 필요할 때
 *
 * 현재는 PlayerController에서 RPC 처리하는 것이 더 간단하고 효율적
 */
UCLASS()
class THIRDMOTION_API UActorSyncManager : public UObject
{
	GENERATED_BODY()

public:
	// 초기화
	void Initialize(UWorld* InWorld);

	// Mesh 동기화 요청 (PlayerController RPC 래핑)
	void RequestMeshSync(AActor* TargetActor, UObject* NewMesh, bool bIsSkeletalMesh);

	// Material 동기화 요청 (PlayerController RPC 래핑)
	void RequestMaterialSync(AActor* TargetActor, int32 MaterialIndex, UMaterialInterface* NewMaterial);

	// 향후 확장 가능한 기능들:
	// - Transform 동기화
	// - Physics 동기화
	// - Custom Property 동기화
	// - Batch 동기화 (여러 변경사항을 한 번에)

private:
	UPROPERTY()
	UWorld* World;

	// PlayerController 참조
	class AThirdMotionPlayerController* GetPlayerController();
};
