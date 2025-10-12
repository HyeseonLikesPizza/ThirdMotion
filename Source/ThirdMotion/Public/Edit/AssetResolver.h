
#pragma once

#include "CoreMinimal.h"
#include "Subsystems/WorldSubsystem.h"
#include "Edit/EditTypes.h"
#include "AssetResolver.generated.h"


UCLASS()
class THIRDMOTION_API UAssetResolver : public UWorldSubsystem
{
	GENERATED_BODY()

public:
	// 테이블 등록 및 인덱스 구축
	UFUNCTION()
	void BuildIndex(UDataTable* DT);
	
	const FLibraryRow* FindRowByTag(const FGameplayTag& Tag) const;

	// 카테고리별 나열 등 UI용도
	void GetRowsByCategory(const FGameplayTag& CategoryTag, TArray<const FLibraryRow*>& OutRows) const;

	
private:
	// 원본 DT 보관
	UPROPERTY()
	TObjectPtr<UDataTable> LibraryTable = nullptr;

	UPROPERTY()
	TMap<FGameplayTag, FLibraryRow> TagToRow;
	
};
