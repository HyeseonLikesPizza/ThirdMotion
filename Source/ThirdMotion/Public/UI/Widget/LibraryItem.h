
#pragma once

#include "CoreMinimal.h"
#include "UI/Widget/BaseWidget.h"
#include "Blueprint/IUserObjectListEntry.h"
#include "Components/Image.h"
#include "Components/TextBlock.h"
#include "Components/Button.h"
#include "LibraryItem.generated.h"

UENUM(BlueprintType)
enum class ELibraryItemType : uint8
{
	Object UMETA(DisplayName = "Object"),
	Material UMETA(DisplayName = "Material"),
	Light UMETA(DisplayName = "Light")
};

/**
 * Data structure for library items
 */
USTRUCT(BlueprintType)
struct FLibraryItemData
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Library")
	FString Name;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Library")
	FString Path;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Library")
	ELibraryItemType Type;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Library")
	UTexture2D* Thumbnail;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Library")
	FString Category;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Library")
	TArray<FString> Tags;

	FLibraryItemData()
		: Name(TEXT("Untitled"))
		, Path(TEXT(""))
		, Type(ELibraryItemType::Object)
		, Thumbnail(nullptr)
		, Category(TEXT("General"))
	{
	}
};

/**
 * Widget for displaying a single library item
 */
UCLASS()
class THIRDMOTION_API ULibraryItem : public UBaseWidget
{
	GENERATED_BODY()

public:
	virtual void NativeConstruct() override;
	virtual void NativeOnListItemObjectSet(UObject* ListItem);
	
	// Widget Components
	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	UImage* ThumbnailImage;

	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	UTextBlock* ItemNameText;

	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	UButton* ItemButton;

	// Set item data
	UFUNCTION(BlueprintCallable, Category = "Library")
	void SetLibraryItemData(const FLibraryItemData& InData);

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Library")
	FLibraryItemData GetLibraryItemData() const { return ItemData; }

	// Events
	DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnLibraryItemClicked, const FLibraryItemData&, ItemData);
	DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnLibraryItemDoubleClicked, const FLibraryItemData&, ItemData);

	UPROPERTY(BlueprintAssignable, Category = "Library Events")
	FOnLibraryItemClicked OnLibraryItemClicked;

	UPROPERTY(BlueprintAssignable, Category = "Library Events")
	FOnLibraryItemDoubleClicked OnLibraryItemDoubleClicked;

protected:
	virtual void NativeOnListItemObjectSet(UObject* ListItem);
	
	UPROPERTY(BlueprintReadOnly, Category = "Library")
	FLibraryItemData ItemData;

	UPROPERTY()
	bool bIsSelected;

	UFUNCTION()
	void OnButtonClicked();

	// Visual state
	UFUNCTION(BlueprintCallable, Category = "Library")
	void SetSelected(bool bSelected);

	UFUNCTION(BlueprintImplementableEvent, Category = "Library")
	void OnItemSelected(bool bSelected);
};
