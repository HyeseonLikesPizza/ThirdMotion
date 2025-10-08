
#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "EditTypes.generated.h"


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

	FEditMeta BuildMetaFromPreset(const FLibraryRow& Row)
	{
		FEditMeta M;
		M.Guid = FGuid::NewGuid();
		M.PresetTag = Row.PresetTag;
		M.DisplayName = Row.DisplayName;
		M.AssetPath = Row.ClassRef.ToSoftObjectPath();
		return M;
	}
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

	FString GuidShort() const
	{
		return Guid.ToString(EGuidFormats::Short).Left(8);
	}
	
};

/*
// TSoftClassPtr<AActor>  →  FSoftClassPath
FSoftClassPath ToPath = ClassPtr.ToSoftObjectPath();

// FSoftClassPath  →  TSoftClassPtr<AActor>
TSoftClassPtr<AActor> ToPtr(ToPath);

// 로딩
UClass* C1 = ClassPtr.LoadSynchronous();
UClass* C2 = ToPath.TryLoadClass<AActor>();
 */