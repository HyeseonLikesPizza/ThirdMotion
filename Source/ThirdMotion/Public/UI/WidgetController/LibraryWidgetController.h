
#pragma once

#include "CoreMinimal.h"
#include "BaseWidgetController.h"
#include "Engine/StreamableManager.h"
#include "LibraryWidgetController.generated.h"

struct FGameplayTag;
class UAssetResolver;
class USceneManager;
struct FLibraryRow;

UCLASS()
class THIRDMOTION_API ULibraryWidgetController : public UBaseWidgetController
{
	GENERATED_BODY()

public:
	void Init() override;

	// 1) 카테고리 별 목록 뽑기 (아이콘 SoftObj 만 비동기)
	void QueryByCategory(const FGameplayTag& Category, TArray<ULibraryItemObject*>& OutItems);

	// 프리뷰 (로컬 고스트)
	void BeginPreview(const FGameplayTag& PresetTag);
	void UpdatePreviewTransform(const FTransform& Xf);
	void CancelPreview();
	void CommitSpawn(const FGameplayTag& PresetTag, const FTransform& FinalTransform);

private:
	// 의존성
	TWeakObjectPtr<UAssetResolver> ResolverCached;
	TWeakObjectPtr<USceneManager> SceneManagerCache;

	// 로컬 프리뷰 고스트
	TWeakObjectPtr<AActor> PreviewGhost;

	// 아이콘 로딩(필요시 사용)
	FStreamableManager Streamable;

private:
	UAssetResolver* GetResolver() const;
	USceneManager* GetSceneManager() const;

	// 고스트 생성·파괴 간단 버전 (복제 X)
	AActor* SpawnPreviewGhost_Simple(const FGameplayTag& PresetTag, const FTransform& Xf);
	void DestroyPreviewGhost();
};
