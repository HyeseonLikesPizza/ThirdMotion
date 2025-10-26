
#pragma once

#include "CoreMinimal.h"
#include "UI/Widget/BaseWidget.h"
#include "MaterialGeneratePanel.generated.h"

class UMaterialPreviewData;

UENUM(BlueprintType)
enum class EMaterialType : uint8
{
	Tire UMETA(DisplayName = "Tire"),
	Foliage UMETA(DisplayName = "Foliage"),
	Video UMETA(DisplayName = "Video"),
	Subsurface UMETA(DisplayName = "Subsurface"),
	CarPaint UMETA(DisplayName = "CarPaint"),
	Water UMETA(DisplayName = "Water"),
	Emissive UMETA(DisplayName = "Emissive"),
	Fabric UMETA(DisplayName = "Fabric"),
	Glass UMETA(DisplayName = "Glass"),
	Standard UMETA(DisplayName = "Standard")
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnMaterialCreated, UMaterialPreviewData*, NewItem);

UCLASS()
class THIRDMOTION_API UMaterialGeneratePanel : public UBaseWidget
{
	GENERATED_BODY()

public:
	virtual bool Initialize() override;
	virtual void NativeConstruct() override;

	// 머티리얼 객체 생성 시 호출되는 델리게이트
	FOnMaterialCreated OnMaterialCreated;

protected:

	// 생성된 모든 버튼을 순서대로 저장할 TArray 선언
	UPROPERTY()
	TArray<class UButton*> MatTypeBtnArray;

	UPROPERTY()
	UEnum* MatEnumPtr;
	//
	// UPROPERTY()
	// TMap<class UButton*, EMaterialType> MatTypeChangeBtnMap;
	//
	//메터리얼 타입
	
	
	//canvas
	UPROPERTY(meta=(BindWidget))
	class UCanvasPanel* MaterialsLeftPanel;
	
	UPROPERTY(meta=(BindWidget))
	class UButton* MaterialCreateBtn;

	UPROPERTY(meta=(BindWidget))
	class UTextBlock* MaterialTypeText;
	
	UPROPERTY(meta=(BindWidget))
	class UButton* MaterialTypeChangePanelOnOffBtn;

	UPROPERTY()
	class UBorder* MaterialTypeChangePanel;

	UPROPERTY()
	class UButton* MaterialTypeChangeBtn;

	UPROPERTY()
	class UListingMaterials* MaterialsSlot;

	UPROPERTY(meta=(BindWidget))
	class UTileView* MaterialTileView;

	UPROPERTY()
	class APreviewImageGenerator* preivewImageGen;
	// UPROPERTY(meta = (BindWidget))
	// class UButton* MaterialTypeSelectionBtn;
	// UPROPERTY(meta = (BindWidget))
	// class UListingMaterials* MaterialsSlot;
	//
	UPROPERTY()
	EMaterialType matTypeSelected;

	
	
private:
	//MaterialTypeChange패널 켜기
	UFUNCTION()
	void TurnOnOffMaterialTypeChangePanel();


	//메터리얼 정렬 필터 버튼 함수

	//안쓰이는 메터리얼 삭제 함수


	UFUNCTION()
	void CreateMaterialTypeEnumBtns();
	

	//Material 생성 함수


	UFUNCTION()
	void OnTireBtnClicked();
	UFUNCTION()
	void OnFoliageBtnClicked();
	UFUNCTION()
	void OnVideoBtnClicked();
	UFUNCTION()
	void OnSubsurfaceBtnClicked();
	UFUNCTION()
	void OnCarPaintBtnClicked();
	UFUNCTION()
	void OnWaterBtnClicked();
	UFUNCTION()
	void OnEmissiveBtnClicked();
	UFUNCTION()
	void OnFabricBtnClicked();
	UFUNCTION()
	void OnGlassBtnClicked();
	UFUNCTION()
	void OnStandardBtnClicked();

	UFUNCTION()
	void OnCreateMaterialBtnClicked();
};
// Tire UMETA(DisplayName = "Tire"),
// Foliage UMETA(DisplayName = "Foliage"),
// Video UMETA(DisplayName = "Subsurface"),
// Subsurface UMETA(DisplayName = "Subsurface"),
// CarPaint UMETA(DisplayName = "CarPaint"),
// Water UMETA(DisplayName = "Water"),
// Emissive UMETA(DisplayName = "Emissive"),
// Fabric UMETA(DisplayName = "Fabric"),
// Glass UMETA(DisplayName = "Glass"),
// Standard UMETA(DisplayName = "Standard")