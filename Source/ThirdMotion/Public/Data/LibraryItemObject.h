
#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "UObject/Object.h"
#include "Edit/EditTypes.h"
#include "LibraryItemObject.generated.h"

UCLASS()
class THIRDMOTION_API ULibraryItemObject : public UObject
{
	GENERATED_BODY()

public:
	UPROPERTY(BlueprintReadOnly)
	FText DisplayName;

	UPROPERTY(BlueprintReadOnly)
	FGameplayTag Tag;

	UPROPERTY(BlueprintReadOnly)
	FGameplayTag NextCategoryTag;

	UPROPERTY(BlueprintReadOnly)
	TSoftObjectPtr<UTexture2D> Icon;

	UPROPERTY(BlueprintReadOnly)
	ELibraryIconType Type;
};
