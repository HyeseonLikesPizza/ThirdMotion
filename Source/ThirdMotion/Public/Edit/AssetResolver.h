
#pragma once

#include "CoreMinimal.h"
#include "Subsystems/WorldSubsystem.h"
#include "Edit/EditTypes.h"
#include "AssetResolver.generated.h"

DECLARE_MULTICAST_DELEGATE(FOnReady)

UCLASS(Config=Game)
class THIRDMOTION_API UAssetResolver : public UWorldSubsystem
{
	GENERATED_BODY()

public:

	UPROPERTY(Config, EditDefaultsOnly, Category = "Library")
	TSoftObjectPtr<UDataTable> LibraryTableAsset;

	bool IsReady() const { return bReady; }
	FOnReady OnReady;
	
	// 테이블 등록 및 인덱스 구축
	UFUNCTION()
	void BuildIndex(UDataTable* DT);
	
	const FLibraryRow* FindRowByTag(const FGameplayTag& Tag) const;

	// 카테고리별 나열 등 UI용도
	void GetRowsByCategory(const FGameplayTag& CategoryTag, TArray<const FLibraryRow*>& OutRows) const;
	const FLibraryRow* GetRowByCategory(const FGameplayTag& CategoryTag) const;
	void GetDirectChildrenCategories(const FGameplayTag& ParentTag, TArray<FGameplayTag>& OutChildren);
	
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;
	virtual void OnWorldBeginPlay(UWorld& InWorld) override;
	
private:
	// 원본 DT 보관
	UPROPERTY()
	TObjectPtr<UDataTable> LibraryTable = nullptr;

	UPROPERTY()
	TMap<FGameplayTag, FLibraryRow> TagToRow;

	bool bReady = false;
	void EnsureLoadedAndBuildIndex();
	
};
