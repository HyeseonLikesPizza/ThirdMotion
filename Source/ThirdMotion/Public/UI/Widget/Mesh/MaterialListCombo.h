
#pragma once

#include "CoreMinimal.h"
#include "Components/Widget.h"
#include "MaterialListCombo.generated.h"

struct FMaterialEntryRow;
template <typename OptionType> class SComboBox;
class UMaterialInstance;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnMaterialPicked, UMaterialInterface*, Material);

UCLASS()
class THIRDMOTION_API UMaterialListCombo : public UWidget
{
	GENERATED_BODY()


public:
	UPROPERTY(BlueprintAssignable)
	FOnMaterialPicked OnMaterialPicked;

	UFUNCTION(BlueprintCallable)
	void SetItems(const TArray<FMaterialEntryRow>& InRows);

	/** 코드에서 머티리얼을 선택 상태로 반영한다. */
	UFUNCTION(BlueprintCallable)
	void SelectMaterial(UMaterialInterface* Material, bool bBroadcastChange = false);

	/** 선택을 해제한다. */
	UFUNCTION(BlueprintCallable)
	void ClearSelection(bool bBroadcastChange = false);

protected:
	virtual TSharedRef<SWidget> RebuildWidget() override;
	virtual void ReleaseSlateResources(bool bReleaseChildren) override;

private:
	TArray<TSharedPtr<FMaterialEntryRow>> Items;
	TSharedPtr<SComboBox<TSharedPtr<FMaterialEntryRow>>> Combo;
	TSharedPtr<FMaterialEntryRow> Current;
	TSoftObjectPtr<UMaterialInterface> PendingSelection;
	bool bBlockSelectionEvent = false;

	TSharedRef<SWidget> GenerateItem(TSharedPtr<FMaterialEntryRow> Row) const;
	void OnChanged(TSharedPtr<FMaterialEntryRow> NewSel, ESelectInfo::Type);
	FText GetSelectedLabel() const;
};
