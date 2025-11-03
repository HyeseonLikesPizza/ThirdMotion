
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

	UPROPERTY(BlueprintReadOnly, ReplicatedUsing=OnRep_Meta)
	FEditMeta R_Meta;

	// Memo Text (복제됨)
	UPROPERTY(ReplicatedUsing=OnRep_MemoText)
	FString R_MemoText;

	UFUNCTION(Server, Reliable)
	void Server_SetTransform(const FTransform& NewWorldTx);

	// 서버에서 실제 적용
	void ApplyTransformAuthoritative(const FTransform& NewWorldTx);

	// Light Intensity 설정 (Server RPC)
	UFUNCTION(Server, Reliable)
	void Server_SetLightIntensity(float NewIntensity);

	// 서버에서 Light Intensity 적용
	void ApplyLightIntensityAuthoritative(float NewIntensity);

	// Memo Text 설정 (Server RPC)
	UFUNCTION(Server, Reliable)
	void Server_SetMemoText(const FString& NewText);

	// 서버에서 Memo Text 적용
	void ApplyMemoTextAuthoritative(const FString& NewText);

protected:
	virtual void BeginPlay() override;
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	UFUNCTION()
	void OnRep_Meta();

	UFUNCTION()
	void OnRep_MemoText();

	// 속성 변경 히스토리
	UPROPERTY(ReplicatedUsing=OnRep_Props)
	TArray<FPropertyDelta> R_PropsAppliedHistory;

	UFUNCTION()
	void OnRep_Props();

	UFUNCTION(BlueprintImplementableEvent)
	ULightComponent* GetLightComponentFromBlueprint(const AActor* Owner) const;

private:
	// 실제 네이티브 컴포넌트에 반영
	void ApplyDeltaToNative(const FPropertyDelta& D);
	void ApplyAllPropsHistory();

	// Memo WidgetComponent 텍스트 업데이트
	void UpdateMemoWidgetText(const FString& NewText);

	UStaticMeshComponent* GetSMC() const;
	ULightComponent* GetLightC() const;
	UCameraComponent* GetCameraC() const;
};
