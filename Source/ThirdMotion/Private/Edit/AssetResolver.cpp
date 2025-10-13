
#include "Edit/AssetResolver.h"
#include "Edit/EditTypes.h"
#include "ThirdMotion/ThirdMotion.h"

void UAssetResolver::BuildIndex(UDataTable* DT)
{
	TagToRow.Empty();
	LibraryTable = DT;
	
	for (auto& It : DT->GetRowMap())
	{
		const FLibraryRow* Row = DT->FindRow<FLibraryRow>(It.Key, TEXT("UAssetResolver::BuildIndex"), true);
		if (!Row) continue;
		
		if (!Row->PresetTag.IsValid())
		{
			PRINTLOG(TEXT("[Resolver] Row %s has invalid PresetTag"), *It.Key.ToString());
			continue;
		}

		if (Row->ClassRef.IsNull())
		{
			PRINTLOG(TEXT("[Resolver] Row %s has null ClassRef (PresetTag : %s)"),
				*It.Key.ToString(), *Row->PresetTag.ToString());
			continue;
		}

		// 태그 충돌
		if (TagToRow.Contains(Row->PresetTag))
		{
			PRINTLOG(TEXT("[Resolver] %s Preset Tag Already Exists"),
				*Row->PresetTag.ToString(), *It.Key.ToString());
			continue;
		}

		TagToRow.Add(Row->PresetTag, *Row);
	}
}

const FLibraryRow* UAssetResolver::FindRowByTag(const FGameplayTag& Tag) const
{
	return TagToRow.Find(Tag);
}

void UAssetResolver::GetRowsByCategory(const FGameplayTag& CategoryTag, TArray<const FLibraryRow*>& OutRows) const
{
	OutRows.Reset();

	const FString Prefix = CategoryTag.ToString() + TEXT(".");
	for (const auto& Pair : TagToRow)
	{
		if (Pair.Key.ToString().StartsWith(Prefix))
		{
			OutRows.Add(&Pair.Value);
		}
	}
}

void UAssetResolver::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);
	bReady = false;
}

void UAssetResolver::OnWorldBeginPlay(UWorld& InWorld)
{
	Super::OnWorldBeginPlay(InWorld);
	EnsureLoadedAndBuildIndex();
}

void UAssetResolver::EnsureLoadedAndBuildIndex()
{
	if (bReady) return;

	if (!LibraryTable && LibraryTableAsset.IsValid())
		LibraryTable = LibraryTableAsset.Get(); // 이미 메모리에 있으면 바로

	if (!LibraryTable && LibraryTableAsset.IsNull() == false)
		LibraryTable = LibraryTableAsset.LoadSynchronous(); // 간단버전: 동기 로드

	if (!LibraryTable)
	{
		PRINTLOG(TEXT("[Resolver] No LibraryTableAsset assigned"));
		return;
	}

	BuildIndex(LibraryTable);
	bReady = true;
	OnReady.Broadcast();
}
