
#pragma once

#include "CoreMinimal.h"
#include "Components/Widget.h"
#include "Data/MeshDataRow.h"
#include "MeshListCombo.generated.h"

template <typename OptionType> class SComboBox;
class UStaticMesh;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnMeshPicked, UStaticMesh*, Mesh);

UCLASS()
class THIRDMOTION_API UMeshListCombo : public UWidget
{
	GENERATED_BODY()

	
public:
	UPROPERTY(BlueprintAssignable)
	FOnMeshPicked OnMeshPicked;

	UFUNCTION(BlueprintCallable)
	void SetItems(const TArray<FMeshDataRow>& InRows);

	/** 코드에서 메시를 선택 상태로 반영한다. */
	UFUNCTION(BlueprintCallable)
	void SelectMesh(UStaticMesh* Mesh, bool bBroadcastChange = false);

	/** 선택을 해제한다. */
	UFUNCTION(BlueprintCallable)
	void ClearSelection(bool bBroadcastChange = false);

protected:
	virtual TSharedRef<SWidget> RebuildWidget() override;
	virtual void ReleaseSlateResources(bool bReleaseChildren) override;

private:
	TArray<TSharedPtr<FMeshDataRow>> Items;
	TSharedPtr<SComboBox<TSharedPtr<FMeshDataRow>>> Combo;
	TSharedPtr<FMeshDataRow> Current;
	TSoftObjectPtr<UStaticMesh> PendingSelection;
	bool bBlockSelectionEvent = false;

	TSharedRef<SWidget> GenerateItem(TSharedPtr<FMeshDataRow> Row) const;
	void OnChanged(TSharedPtr<FMeshDataRow> NewSel, ESelectInfo::Type);
	FText GetSelectedLabel() const;
	
};
