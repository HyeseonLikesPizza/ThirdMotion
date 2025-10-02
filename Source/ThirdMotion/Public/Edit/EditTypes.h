
#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "EditTypes.generated.h"



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

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FSoftObjectPath AssetPath;

	FString GuidShort() const
	{
		return Guid.ToString(EGuidFormats::Short).Left(8);
	}
	
};

