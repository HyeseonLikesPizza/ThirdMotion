// Fill out your copyright notice in the Description page of Project Settings.

#include "UI/Widget/AssetPickerWidget.h"
#include "Components/ListView.h"
#include "Components/TextBlock.h"
#include "Components/Button.h"
#include "Components/EditableTextBox.h"
#include "Components/Image.h"
#include "Engine/StaticMesh.h"
#include "Engine/SkeletalMesh.h"
#include "Materials/MaterialInterface.h"
#include "AssetRegistry/AssetRegistryModule.h"
#include "AssetRegistry/AssetData.h"
#include "UObject/UObjectIterator.h"

// ==================== UAssetPickerWidget ====================

void UAssetPickerWidget::NativeConstruct()
{
	Super::NativeConstruct();

	// 버튼 이벤트 바인딩
	if (CloseButton)
	{
		CloseButton->OnClicked.AddDynamic(this, &UAssetPickerWidget::OnCloseButtonClicked);
	}

	// 검색 박스 이벤트 바인딩
	if (SearchBox)
	{
		SearchBox->OnTextChanged.AddDynamic(this, &UAssetPickerWidget::OnSearchTextChanged);
	}

	// ListView 이벤트 바인딩
	if (AssetListView)
	{
		AssetListView->OnItemClicked().AddUObject(this, &UAssetPickerWidget::OnAssetItemClicked);

		// Entry 위젯 생성 시 콜백 설정 (주석 처리 - 크래시 방지)
		// TODO: 올바른 델리게이트 시그니처 확인 필요
		// AssetListView->OnEntryWidgetGenerated().AddUObject(this, &UAssetPickerWidget::OnEntryWidgetGenerated);
	}
}

void UAssetPickerWidget::SetAssetClass(UClass* InAssetClass)
{
	AssetClass = InAssetClass;

	// 타이틀 업데이트
	if (TitleText && AssetClass)
	{
		FString Title = FString::Printf(TEXT("Select %s"), *AssetClass->GetName());
		TitleText->SetText(FText::FromString(Title));
	}
}

void UAssetPickerWidget::LoadAssets()
{
	if (!AssetClass)
	{
		UE_LOG(LogTemp, Warning, TEXT("AssetClass is not set!"));
		return;
	}

	AllAssets.Empty();
	FilteredAssets.Empty();

	// 에셋 로드
	LoadAllAssetsOfClass(AssetClass);

	// ListView 업데이트
	UpdateAssetList();

	UE_LOG(LogTemp, Log, TEXT("Loaded %d assets of class %s"), AllAssets.Num(), *AssetClass->GetName());
}

void UAssetPickerWidget::FilterAssets(const FString& SearchText)
{
	FilteredAssets.Empty();

	if (SearchText.IsEmpty())
	{
		FilteredAssets = AllAssets;
	}
	else
	{
		for (UObject* Asset : AllAssets)
		{
			if (Asset && Asset->GetName().Contains(SearchText))
			{
				FilteredAssets.Add(Asset);
			}
		}
	}

	UpdateAssetList();

	UE_LOG(LogTemp, Log, TEXT("Filtered assets: %d results"), FilteredAssets.Num());
}

void UAssetPickerWidget::OnSearchTextChanged(const FText& Text)
{
	FilterAssets(Text.ToString());
}

void UAssetPickerWidget::OnAssetItemClicked(UObject* Item)
{
	if (UAssetItemData* ItemData = Cast<UAssetItemData>(Item))
	{
		if (ItemData->Asset)
		{
			UE_LOG(LogTemp, Log, TEXT("Asset clicked: %s"), *ItemData->Asset->GetName());

			// 에셋 선택 이벤트 브로드캐스트
			OnAssetPicked.Broadcast(ItemData->Asset);

			// AssetPicker 닫기
			RemoveFromParent();
		}
	}
}

void UAssetPickerWidget::OnCloseButtonClicked()
{
	RemoveFromParent();
}

void UAssetPickerWidget::OnEntryWidgetGenerated(UUserWidget& Widget)
{
	// NOTE: 이 함수는 현재 사용되지 않음 (델리게이트 바인딩 주석 처리됨)
	// ListView에서 AssetItemWidget이 생성될 때 호출되어야 하지만
	// UE 5.6에서 델리게이트 시그니처 문제로 크래시 발생
	// 대신 AssetItemWidget이 GetOuter()로 부모를 찾음

	if (UAssetItemWidget* ItemWidget = Cast<UAssetItemWidget>(&Widget))
	{
		// OwningPicker 설정
		ItemWidget->SetOwningPicker(this);
		UE_LOG(LogTemp, Log, TEXT("AssetItemWidget generated and OwningPicker set"));
	}
}

void UAssetPickerWidget::LoadAllAssetsOfClass(UClass* InClass)
{
	if (!InClass)
		return;

	// AssetRegistry를 사용하여 에셋 로드
	FAssetRegistryModule& AssetRegistryModule = FModuleManager::LoadModuleChecked<FAssetRegistryModule>("AssetRegistry");
	IAssetRegistry& AssetRegistry = AssetRegistryModule.Get();

	// 에셋 검색 필터 설정
	FARFilter Filter;
	Filter.ClassPaths.Add(InClass->GetClassPathName());
	Filter.bRecursiveClasses = true;

	// 에셋 검색
	TArray<FAssetData> AssetDataList;
	AssetRegistry.GetAssets(Filter, AssetDataList);

	// 에셋 로드
	for (const FAssetData& AssetData : AssetDataList)
	{
		UObject* Asset = AssetData.GetAsset();
		if (Asset)
		{
			AllAssets.Add(Asset);
			FilteredAssets.Add(Asset);
		}
	}

	UE_LOG(LogTemp, Log, TEXT("Found %d assets of type %s"), AllAssets.Num(), *InClass->GetName());
}

void UAssetPickerWidget::UpdateAssetList()
{
	if (!AssetListView)
		return;

	AssetListView->ClearListItems();

	for (UObject* Asset : FilteredAssets)
	{
		if (Asset)
		{
			// AssetItemData 생성
			UAssetItemData* ItemData = NewObject<UAssetItemData>(this);
			ItemData->SetAsset(Asset);

			// ListView에 추가
			AssetListView->AddItem(ItemData);
		}
	}

	UE_LOG(LogTemp, Log, TEXT("AssetListView updated with %d items"), FilteredAssets.Num());
}

// ==================== UAssetItemWidget ====================

void UAssetItemWidget::NativeConstruct()
{
	Super::NativeConstruct();

	// 버튼 이벤트 바인딩
	if (SelectButton)
	{
		SelectButton->OnClicked.AddDynamic(this, &UAssetItemWidget::OnSelectButtonClicked);
	}
}

void UAssetItemWidget::SetOwningPicker(UAssetPickerWidget* InPicker)
{
	OwningPicker = InPicker;
}

void UAssetItemWidget::NativeOnListItemObjectSet(UObject* ListItem)
{
	ItemData = Cast<UAssetItemData>(ListItem);

	if (ItemData && ItemData->Asset)
	{
		// 에셋 이름 설정
		if (AssetNameText)
		{
			AssetNameText->SetText(FText::FromString(ItemData->AssetName));
		}

		// 에셋 경로 설정
		if (AssetPathText)
		{
			AssetPathText->SetText(FText::FromString(ItemData->AssetPath));
		}

		// 썸네일 로드
		LoadThumbnail();
	}
}

void UAssetItemWidget::OnSelectButtonClicked()
{
	if (ItemData && ItemData->Asset)
	{
		UE_LOG(LogTemp, Log, TEXT("Asset selected: %s"), *ItemData->Asset->GetName());

		// OwningPicker를 통해 이벤트 브로드캐스트
		if (OwningPicker)
		{
			OwningPicker->OnAssetItemClicked(ItemData);
			UE_LOG(LogTemp, Log, TEXT("AssetPicker event broadcasted"));
		}
		else
		{
			// OwningPicker가 설정되지 않은 경우 수동으로 찾기
			UE_LOG(LogTemp, Warning, TEXT("OwningPicker is null - trying to find it manually"));

			// GetOuter를 통해 상위 객체 탐색
			UObject* Outer = GetOuter();
			while (Outer)
			{
				if (UAssetPickerWidget* Picker = Cast<UAssetPickerWidget>(Outer))
				{
					Picker->OnAssetItemClicked(ItemData);
					UE_LOG(LogTemp, Log, TEXT("Found AssetPickerWidget via GetOuter"));
					return;
				}
				Outer = Outer->GetOuter();
			}

			UE_LOG(LogTemp, Error, TEXT("Could not find AssetPickerWidget"));
		}
	}
}

void UAssetItemWidget::LoadThumbnail()
{
	if (!AssetThumbnail || !ItemData || !ItemData->Asset)
		return;

	// 썸네일 로드 (간단한 버전)
	// 실제로는 AssetRegistry의 GetAssetThumbnail을 사용하거나
	// Material의 경우 BaseColor 텍스처를 사용할 수 있음

	UE_LOG(LogTemp, Log, TEXT("Loading thumbnail for: %s"), *ItemData->AssetName);
}
