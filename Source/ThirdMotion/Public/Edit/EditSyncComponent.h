
#pragma once

#include "CoreMinimal.h"
#include "EditTypes.h"
#include "Components/ActorComponent.h"
#include "EditSyncComponent.generated.h"

class UStaticMeshComponent;
class UCameraComponent;
class ULightComponent;

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class THIRDMOTION_API UEditSyncComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UEditSyncComponent();

	// 서버: 스폰 직후 프리셋에서 메타 채움
	void InitMetaFromPreset(const struct FLibraryRow& Row);

	// 서버: 세이브 로드 시 메타 주입(Unsafe = 서버에서만)
	void SetMeta_Unsafe(const FEditMeta& InMeta);

	const FEditMeta& GetMeta() const { return R_Meta; }

	// 저장,복원
	void BuildSave(struct FActorSave& Out) const;
	void ApplySave(const struct FActorSave& In);

	// 로컬 전용 상태(복제X)
	UPROPERTY(Transient)
	bool bSelectedLocal = false;

	// SceneManager만 호출하는 서버 내부용 속성 적용 API
	void ServerApplyPropertyDelta_Internal(const FPropertyDelta& D);

protected:
	virtual void BeginPlay() override;
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	UPROPERTY(ReplicatedUsing=OnRep_Meta)
	FEditMeta R_Meta;

	UFUNCTION()
	void OnRep_Meta();

	// 속성 변경 히스토리
	UPROPERTY(ReplicatedUsing=OnRep_Props)
	TArray<FPropertyDelta> R_PropsAppliedHistory;

	UFUNCTION()
	void OnRep_Props();

private:
	// 실제 네이티브 컴포넌트에 반영
	void ApplyDeltaToNative(const FPropertyDelta& D);
	void ApplyAllPropsHistory();

	UStaticMeshComponent* GetSMC() const;
	ULightComponent* GetLightC() const;
	UCameraComponent* GetCameraC() const;
};
