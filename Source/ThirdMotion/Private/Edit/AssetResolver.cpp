
#include "Edit/AssetResolver.h"
#include "Edit/EditTypes.h"
#include "ThirdMotion/ThirdMotion.h"

void UAssetResolver::GetAllStaticMeshRows(TArray<const FMeshDataRow*>& OutRows)
{
	if (!MeshTable) return;
	if (MeshDataArray.IsEmpty())
	{
		CacheMeshData();
	}
	
	OutRows.Reset();
	OutRows.Reserve(MeshDataArray.Num());
	
	for (const FMeshDataRow* Row : MeshDataArray)
	{
		OutRows.Add(Row);
	}
	
}

void UAssetResolver::CacheMeshData()
{
	MeshDataArray.Reset();
	if (!MeshTable) return;
	
	MeshTable->GetAllRows<FMeshDataRow>(TEXT("AssetResolver::GetAllStaticMeshRows"), MeshDataArray);
}

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

const FLibraryRow* UAssetResolver::GetRowByCategory(const FGameplayTag& CategoryTag) const
{
	if (const FLibraryRow* Found = TagToRow.Find(CategoryTag))
		return Found;
	return nullptr;
}

void UAssetResolver::GetDirectChildrenCategories(const FGameplayTag& ParentTag, TArray<FGameplayTag>& OutChildren)
{
	OutChildren.Reset();
	if (!ParentTag.IsValid()) return;

	const FString ParentStr = ParentTag.ToString();
	const FString Prefix    = ParentStr + TEXT(".");   

	TSet<FGameplayTag> Unique;

	for (const auto& Pair : TagToRow)
	{
		const FString RowStr = Pair.Key.ToString();

		// 1) 부모의 하위가 아니면 스킵
		if (!RowStr.StartsWith(Prefix)) continue;

		// 2) Prefix 이후 첫 '.' 위치를 찾아서 "직계" 경계 판단
		int32 FirstDotAfterPrefix = INDEX_NONE;
		RowStr.FindChar('.', FirstDotAfterPrefix); // 이건 문자열 처음부터라서 안 맞음

		// 올바른 방식: Prefix 다음부터 검색
		FirstDotAfterPrefix = RowStr.Find(TEXT("."), ESearchCase::CaseSensitive, ESearchDir::FromStart, Prefix.Len());

		FString ChildStr;
		if (FirstDotAfterPrefix == INDEX_NONE)
		{
			// 더 이상 점이 없으면 RowStr 전체가 직계 (예: "Category.Object")
			ChildStr = RowStr;
		}
		else
		{
			// Prefix부터 다음 점 전까지 = 직계 (예: "Category.Object")
			ChildStr = RowStr.Left(FirstDotAfterPrefix);
		}

		// 3) GameplayTag로 변환(미등록일 수 있으니 false)
		const FGameplayTag ChildTag = FGameplayTag::RequestGameplayTag(FName(*ChildStr), /*ErrorIfNotFound*/ false);
		if (ChildTag.IsValid() && ChildTag != ParentTag)
		{
			Unique.Add(ChildTag);
		}
	}

	OutChildren = Unique.Array();

	// 보기 좋게 정렬(선택)
	OutChildren.Sort([](const FGameplayTag& A, const FGameplayTag& B){
		return A.ToString() < B.ToString();
	});
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

	if (!MeshTable && MeshRowTableAsset.IsValid())
		MeshTable = MeshRowTableAsset.Get();

	if (!MeshTable && MeshRowTableAsset.IsNull() == false)
		MeshTable = MeshRowTableAsset.LoadSynchronous();

	BuildIndex(LibraryTable);
	bReady = true;
	OnReady.Broadcast();
}
