
#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "Blueprint/UserWidget.h"
#include "CategoryButton.generated.h"

UCLASS()
class THIRDMOTION_API UCategoryButton : public UUserWidget
{
	GENERATED_BODY()

public:
	virtual void NativeConstruct() override;

	UPROPERTY(meta = (BindWidget), BlueprintReadWrite)
	class UTextBlock* LabelText;

	UPROPERTY(meta = (BindWidget))
	class UButton* Button;
	
	// 전체 태그
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Category")
	FGameplayTag CategoryTag;

	// 현재 위치일 때 비활성/파란색 등
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Category")
	bool bIsActive = false; 

	DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnCategoryClicked, FGameplayTag, CategoryTag);

	UPROPERTY(BlueprintAssignable, Category="Category")
	FOnCategoryClicked OnCategoryClicked;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Style")
	FLinearColor ActiveColor = FLinearColor(0.30f, 0.64f, 1.0f);      // 파랑
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Style")
	FLinearColor NormalColor = FLinearColor(0.85f, 0.85f, 0.85f);     // 회색

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Style")
	FLinearColor LibraryColor = FLinearColor(1.f, 1.f, 1.f);     // 회색

public:
	UFUNCTION(BlueprintCallable)
	void SetText(const FText& InText);

	UFUNCTION(BlueprintCallable)
	void SetActive(bool bNewActive);

private:
	UFUNCTION()
	void HandleClicked();
};
