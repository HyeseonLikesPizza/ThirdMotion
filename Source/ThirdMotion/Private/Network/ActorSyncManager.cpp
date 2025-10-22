// 향후 NetworkManager가 필요할 경우를 대비한 예시
// 현재는 사용하지 않음 - PlayerController에서 직접 처리

#include "Network/ActorSyncManager.h"
#include "Framework/ThirdMotionPlayerController.h"
#include "Engine/World.h"

void UActorSyncManager::Initialize(UWorld* InWorld)
{
	World = InWorld;
}

void UActorSyncManager::RequestMeshSync(AActor* TargetActor, UObject* NewMesh, bool bIsSkeletalMesh)
{
	// PlayerController의 RPC를 래핑
	if (AThirdMotionPlayerController* PC = GetPlayerController())
	{
		PC->Server_UpdateActorMesh(TargetActor, NewMesh, bIsSkeletalMesh);
	}
}

void UActorSyncManager::RequestMaterialSync(AActor* TargetActor, int32 MaterialIndex, UMaterialInterface* NewMaterial)
{
	// PlayerController의 RPC를 래핑
	if (AThirdMotionPlayerController* PC = GetPlayerController())
	{
		PC->Server_UpdateActorMaterial(TargetActor, MaterialIndex, NewMaterial);
	}
}

AThirdMotionPlayerController* UActorSyncManager::GetPlayerController()
{
	if (!World)
		return nullptr;

	return Cast<AThirdMotionPlayerController>(World->GetFirstPlayerController());
}
