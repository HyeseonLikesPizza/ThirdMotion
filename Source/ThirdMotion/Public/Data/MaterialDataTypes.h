
#pragma once

#include "CoreMinimal.h"
#include "MaterialDataTypes.generated.h"



UENUM(BlueprintType)
enum class EMaterialEntryKind : uint8
{
	Asset,          // 에셋 그대로 사용
	DynamicRecipe   // 레시피로 런타임에 MID 생성
};

USTRUCT(BlueprintType)
struct FMaterialParamSet
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TMap<FName, float> ScalarParams;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TMap<FName, FLinearColor> VectorParams;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TMap<FName, TSoftObjectPtr<UTexture>> TextureParams;
};

USTRUCT(BlueprintType)
struct FMaterialEntryRow : public FTableRowBase
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FName EntryName;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	EMaterialEntryKind Kind = EMaterialEntryKind::Asset;

	// Kind == Asset: 바로 쓸 에셋(UMaterial 또는 UMaterialInstanceConstant)
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TSoftObjectPtr<UMaterialInterface> AssetRef;

	// Kind == DynamicRecipe: MID를 만들기 위한 "베이스 에셋"
	// ※ 여기엔 MID가 아니라 경로가 있는 Base Material / MIC가 들어감
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TSoftObjectPtr<UMaterialInterface> BaseAssetRef;

	// 레시피 파라미터(런타임에 MID 생성시 적용)
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FMaterialParamSet Params;

	// ===== 런타임 캐시(직렬화 X) =====
	// 실제 사용 중의 런타임 머티리얼 (대개 MID)
	UPROPERTY(Transient)
	TWeakObjectPtr<UMaterialInstanceDynamic> RuntimeMID;

	// 썸네일 텍스처(런타임 생성)
	UPROPERTY(Transient)
	UTexture2D* PreviewTexture = nullptr;
};