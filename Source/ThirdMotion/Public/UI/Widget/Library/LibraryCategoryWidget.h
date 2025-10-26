
#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "Blueprint/UserWidget.h"
#include "LibraryCategoryWidget.generated.h"

class UHorizontalBox;
class UCategoryButton;
class ULibraryWidgetController;

UCLASS()
class THIRDMOTION_API ULibraryCategoryWidget : public UUserWidget
{
	GENERATED_BODY()

public:

	void SetController(ULibraryWidgetController* InController);
	
	UPROPERTY(meta=(BindWidget))
	UHorizontalBox* BreadcrumbBox;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Breadcrumb")
	FGameplayTag CurrentTag;

	DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnCrumbClicked, int32, Index, const TArray<FString>&, PathUpTo);

	UPROPERTY(BlueprintAssignable, Category="Breadcrumb")
	FOnCrumbClicked OnCrumbClicked;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Style")
	FLinearColor ActiveColor = FLinearColor(0.30f, 0.64f, 1.0f);      // 파랑
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Style")
	FLinearColor NormalColor = FLinearColor(0.85f, 0.85f, 0.85f);     // 회색
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Style")
	FLinearColor SeparatorColor = FLinearColor(0.70f, 0.70f, 0.70f);  // 연회색

	/* 구분자 기호 */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Style")
    FString SeparatorSymbol = TEXT(" › ");

	DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnCrumbChosen, FGameplayTag, CategoryTag);
	
	UPROPERTY(BlueprintAssignable, Category="Breadcrumb")
	FOnCrumbChosen OnCrumbChosen;

	/* Category Button 클래스 */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Category")
	TSubclassOf<UCategoryButton> CategoryBtnClass;

public:
	
	/** 외부에서 현재 경로를 태그로 지정 */
	UFUNCTION(BlueprintCallable, Category="Breadcrumb")
	void SetFromTag(FGameplayTag InTag);

	/** 외부에서 경로 파츠로 지정하고 태그 구성 (ex: ["Library","Objects","Furniture"]) */
	UFUNCTION(BlueprintCallable, Category="Breadcrumb")
	void SetFromParts(const TArray<FString>& Parts);
	
protected:
	virtual void NativeConstruct() override;

private:

	UPROPERTY()
	ULibraryWidgetController* Controller;
	
	/** 생성된 버튼을 보관(GC 보호) */
	UPROPERTY()
	TArray<UCategoryButton*> Buttons;
	
	void Rebuild();
	void AddCrumb(const FGameplayTag& Tag, const FString& Title, bool bActive);
	
	UFUNCTION()
	void HandleCrumbClicked(FGameplayTag Tag);

	/** 유틸: 태그 -> 파츠 분리 */
	static void SplitTag(const FGameplayTag& Tag, TArray<FString>& OutParts);

	/** 유틸: 0..UpToIdx 까지 파츠를 합쳐 부분 태그 생성 */
	static FGameplayTag MakePartialTag(const TArray<FString>& Parts, int32 UpToIdx);
};
