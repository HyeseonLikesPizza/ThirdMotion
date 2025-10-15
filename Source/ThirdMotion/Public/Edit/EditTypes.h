
#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "EditTypes.generated.h"

UENUM(BlueprintType)
enum class ELibraryIconType : uint8
{
	Category,
	Preset
};

UENUM(BlueprintType)
enum class EPresetType : uint8
{
	Mesh,
	Light,
	Camera
};

UENUM(BlueprintType)
enum class EPropOp : uint8
{
	SetFloat,
	SetInt,
	SetColor,
	SetName,
	SetObject,
	SetTransform
};

USTRUCT(BlueprintType)
struct FPropertyDelta
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FGameplayTag PropertyTag;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	EPropOp Op = EPropOp::SetFloat;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 IntParam = 0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float FloatParam = 0.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FLinearColor ColorParam = FLinearColor::White;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FName NameParam = FName("");

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FSoftObjectPath ObjectPath;
};



// 뷰포트 상에 해당 프리셋 추가 시 사용되는 구조체
USTRUCT(BlueprintType)
struct FEditMeta
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FGuid Guid;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FGameplayTag PresetTag;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FName DisplayName;

};

// 라이브러리 프리셋 구조체
USTRUCT(BlueprintType)
struct FLibraryRow : public FTableRowBase
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere)
	FName DisplayName;

	UPROPERTY(EditAnywhere)
	FGameplayTag PresetTag;

	UPROPERTY(EditAnywhere)
	TSoftClassPtr<AActor> ClassRef;

	UPROPERTY(EditAnywhere)
	TSoftObjectPtr<UTexture2D> Icon;

	UPROPERTY(EditAnywhere)
	EPresetType PresetType;

	

	FEditMeta BuildMetaFromPreset(const FLibraryRow& Row)
	{
		FEditMeta M;
		M.Guid = FGuid::NewGuid();
		M.PresetTag = Row.PresetTag;
		M.DisplayName = Row.DisplayName;
		return M;
	}
};

