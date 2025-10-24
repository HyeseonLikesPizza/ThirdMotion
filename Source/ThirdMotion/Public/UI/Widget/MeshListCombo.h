
#pragma once

#include "CoreMinimal.h"
#include "Components/Widget.h"
#include "Data/MeshDataRow.h"
#include "MeshListCombo.generated.h"

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

protected:
	virtual TSharedRef<SWidget> RebuildWidget() override;
	virtual void ReleaseSlateResources(bool bReleaseChildren) override;

private:
	TArray<TSharedPtr<FMeshDataRow>> Items;
	TSharedPtr<SComboBox<TSharedPtr<FMeshDataRow>>> Combo;
	TSharedPtr<FMeshDataRow> Current;

	TSharedRef<SWidget> GenerateItem(TSharedPtr<FMeshDataRow> Row) const;
	void OnChanged(TSharedPtr<FMeshDataRow> NewSel, ESelectInfo::Type);
	FText GetSelectedLabel() const;
};
