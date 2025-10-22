// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UI/Widget/BaseWidget.h"
#include "Components/ListView.h"
#include "Components/TextBlock.h"
#include "Components/Button.h"
#include "Components/EditableTextBox.h"
#include "Components/Image.h"
#include "AssetPickerWidget.generated.h"

class UAssetItemWidget;

// 에셋 선택 델리게이트
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnAssetPicked, UObject*, PickedAsset);

/**
 * AssetPickerWidget
 * 언리얼 에디터의 Content Browser와 유사한 에셋 선택 위젯
 * StaticMesh, SkeletalMesh, Material 등을 선택할 수 있음
 */
UCLASS()
class THIRDMOTION_API UAssetPickerWidget : public UBaseWidget
{
	GENERATED_BODY()

public:
	virtual void NativeConstruct() override;

	// 에셋 타입 설정 (UStaticMesh, UMaterial 등)
	UFUNCTION(BlueprintCallable, Category = "Asset Picker")
	void SetAssetClass(UClass* InAssetClass);

	// 에셋 목록 로드 및 표시
	UFUNCTION(BlueprintCallable, Category = "Asset Picker")
	void LoadAssets();

	// 검색 필터 적용
	UFUNCTION(BlueprintCallable, Category = "Asset Picker")
	void FilterAssets(const FString& SearchText);

	// Widget Components
	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	UTextBlock* TitleText;

	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	UEditableTextBox* SearchBox;

	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	UListView* AssetListView;

	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	UButton* CloseButton;

	// AssetItem 위젯 클래스
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Asset Picker")
	TSubclassOf<UUserWidget> AssetItemWidgetClass;

	// 에셋 선택 이벤트
	UPROPERTY(BlueprintAssignable, Category = "Asset Picker")
	FOnAssetPicked OnAssetPicked;

protected:
	// 에셋 타입 (UStaticMesh, UMaterial 등)
	UPROPERTY()
	UClass* AssetClass;

	// 로드된 모든 에셋
	UPROPERTY()
	TArray<UObject*> AllAssets;

	// 필터링된 에셋
	UPROPERTY()
	TArray<UObject*> FilteredAssets;

private:
	UFUNCTION()
	void OnSearchTextChanged(const FText& Text);

	UFUNCTION()
	void OnAssetItemClicked(UObject* Item);

	UFUNCTION()
	void OnCloseButtonClicked();

	// Entry 위젯이 생성될 때 호출
	void OnEntryWidgetGenerated(UUserWidget& Widget);

	// 프로젝트 내 모든 에셋 로드
	void LoadAllAssetsOfClass(UClass* InClass);

	// ListView 업데이트
	void UpdateAssetList();
};

/**
 * AssetItemData
 * ListView에서 사용할 에셋 데이터 래퍼
 */
UCLASS(BlueprintType)
class THIRDMOTION_API UAssetItemData : public UObject
{
	GENERATED_BODY()

public:
	UPROPERTY(BlueprintReadWrite, Category = "Asset Item")
	UObject* Asset;

	UPROPERTY(BlueprintReadWrite, Category = "Asset Item")
	FString AssetName;

	UPROPERTY(BlueprintReadWrite, Category = "Asset Item")
	FString AssetPath;

	void SetAsset(UObject* InAsset)
	{
		Asset = InAsset;
		if (Asset)
		{
			AssetName = Asset->GetName();
			AssetPath = Asset->GetPathName();
		}
	}
};

/**
 * AssetItemWidget
 * AssetPicker의 ListView에서 각 에셋을 표시하는 위젯
 */
UCLASS()
class THIRDMOTION_API UAssetItemWidget : public UBaseWidget, public IUserObjectListEntry
{
	GENERATED_BODY()

public:
	virtual void NativeConstruct() override;
	virtual void NativeOnListItemObjectSet(UObject* ListItem) override;

	// AssetPicker 참조 설정
	void SetOwningPicker(UAssetPickerWidget* InPicker);

	// Widget Components
	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	UTextBlock* AssetNameText;

	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	UTextBlock* AssetPathText;

	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	UButton* SelectButton;

	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	UImage* AssetThumbnail;

protected:
	UPROPERTY()
	UAssetItemData* ItemData;

	UPROPERTY()
	UAssetPickerWidget* OwningPicker;

private:
	UFUNCTION()
	void OnSelectButtonClicked();

	// 썸네일 로드
	void LoadThumbnail();
};
