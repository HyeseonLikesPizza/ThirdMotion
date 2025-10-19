#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Blueprint/IUserObjectListEntry.h"
#include "Data/SceneItemData.h"
#include "SceneItemWidget.generated.h"

class UTextBlock;
class UButton;
class USceneController;


/**
 * SceneItemWidget - Model 역할
 * Outline에 출력되는 데이터를 가져와서 SceneList에 설정 처리
 * SceneController를 통해 모든 액션 수행
 */
UCLASS()
class THIRDMOTION_API USceneItemWidget : public UUserWidget, public IUserObjectListEntry
{
	GENERATED_BODY()

public:
	// SceneController 설정
	void SetSceneController(USceneController* InController);

protected:
	virtual void NativeConstruct() override;
	virtual void NativeDestruct() override;
	virtual void NativeOnListItemObjectSet(UObject* ListItemObject) override;

	UPROPERTY(meta = (BindWidget))
	UTextBlock* ItemLabel;

	UPROPERTY(meta = (BindWidget))
	UTextBlock* ItemType;

	UPROPERTY(meta = (BindWidget))
	UButton* VisibilityButton;

	UPROPERTY()
	USceneItemData* ItemData;

	// SceneController 참조 (모든 액션은 Controller를 통해 수행)
	UPROPERTY()
	USceneController* SceneController;

private:
	UFUNCTION()
	void OnModelDataChanged(USceneItemData* ChangedData);

	void UpdateView();

	UFUNCTION()
	void OnVisibilityButtonClicked();

	UFUNCTION()
	void OnItemClicked();
};
