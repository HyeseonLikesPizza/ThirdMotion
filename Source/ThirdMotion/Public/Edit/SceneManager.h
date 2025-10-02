
#pragma once

#include "CoreMinimal.h"
#include "Subsystems/WorldSubsystem.h"
#include "SceneManager.generated.h"

UCLASS()
class THIRDMOTION_API USceneManager : public UWorldSubsystem
{
	GENERATED_BODY()

public:
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;
	virtual void Deinitialize() override;

	// 서버만 호출 허용
	bool IsAuthority() const;

	// Guid로 액터 찾기
	AActor* FindByGuid(const FGuid& ID) const;;

	// 스폰/변경/삭제
	//void SpawnFromPreset(FName )
	
};
