// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/Panel/MaterialGeneratePanel.h"

#include "Blueprint/WidgetTree.h"
#include "Components/Border.h"
#include "Components/Button.h"
#include "Components/CanvasPanel.h"
#include "Components/CanvasPanelSlot.h"
#include "Components/TextBlock.h"
#include "Components/TileView.h"
#include "Components/VerticalBox.h"
#include "Components/VerticalBoxSlot.h"
#include "Data/MaterialThumbnailData.h"
#include "UI/Widget/ListingMaterials.h"

class UCanvasPanelSlot;
class UVerticalBoxSlot;
class UButton;
class UTextBlock;

bool UMaterialGeneratePanel::Initialize()
{
	// 1. 부모 초기화
	bool bSuccess = Super::Initialize();
	if (!bSuccess) return false;

	// 2. 추가 초기화
	CreateMaterialTypeEnumBtns();

	MatTypeBtnArray[0]->OnClicked.AddDynamic(this,&UMaterialGeneratePanel::OnTireBtnClicked);
	MatTypeBtnArray[1]->OnClicked.AddDynamic(this,&UMaterialGeneratePanel::OnFoliageBtnClicked);
	MatTypeBtnArray[2]->OnClicked.AddDynamic(this,&UMaterialGeneratePanel::OnVideoBtnClicked);
	MatTypeBtnArray[3]->OnClicked.AddDynamic(this,&UMaterialGeneratePanel::OnSubsurfaceBtnClicked);
	MatTypeBtnArray[4]->OnClicked.AddDynamic(this,&UMaterialGeneratePanel::OnCarPaintBtnClicked);
	MatTypeBtnArray[5]->OnClicked.AddDynamic(this,&UMaterialGeneratePanel::OnWaterBtnClicked);
	MatTypeBtnArray[6]->OnClicked.AddDynamic(this,&UMaterialGeneratePanel::OnEmissiveBtnClicked);
	MatTypeBtnArray[7]->OnClicked.AddDynamic(this,&UMaterialGeneratePanel::OnFabricBtnClicked);
	MatTypeBtnArray[8]->OnClicked.AddDynamic(this,&UMaterialGeneratePanel::OnGlassBtnClicked);
	MatTypeBtnArray[9]->OnClicked.AddDynamic(this,&UMaterialGeneratePanel::OnStandardBtnClicked);

	MaterialCreateBtn->OnClicked.AddDynamic(this, &UMaterialGeneratePanel::OnCreateMaterialBtnClicked);
	
	
	// 3. 성공 반환
	return true;
}

void UMaterialGeneratePanel::NativeConstruct()
{
	Super::NativeConstruct();

	MaterialTypeChangePanelOnOffBtn->OnClicked.AddDynamic(this, &UMaterialGeneratePanel::TurnOnOffMaterialTypeChangePanel);
}

void UMaterialGeneratePanel::TurnOnOffMaterialTypeChangePanel()
{
	if (MaterialTypeChangePanel->GetVisibility() == ESlateVisibility::Visible)
	{
		MaterialTypeChangePanel->SetVisibility(ESlateVisibility::Hidden);
	}
	else
	{
		MaterialTypeChangePanel->SetVisibility(ESlateVisibility::Visible);
	}
}


void UMaterialGeneratePanel::CreateMaterialTypeEnumBtns()
{

	MatEnumPtr = StaticEnum<EMaterialType>();


	// 1️⃣ Border 생성
	MaterialTypeChangePanel = WidgetTree->ConstructWidget<UBorder>(UBorder::StaticClass());
	MaterialTypeChangePanel->SetBrushColor(FLinearColor::Black); // 검은색 배경
	MaterialTypeChangePanel->SetPadding(FMargin(5.f)); // 버튼과 여백

	// 2️⃣ VerticalBox 생성
	UVerticalBox* ButtonBox = WidgetTree->ConstructWidget<UVerticalBox>(UVerticalBox::StaticClass());

	// 3️⃣ Border 안에 VerticalBox 추가
	MaterialTypeChangePanel->AddChild(ButtonBox);

	// 4️⃣ Canvas에 Border 추가
	UCanvasPanelSlot* CanvasSlot = MaterialsLeftPanel->AddChildToCanvas(MaterialTypeChangePanel);
	CanvasSlot->SetAutoSize(true);
	CanvasSlot->SetPosition(FVector2D(18, -480));

	// 🔹 3️⃣ Enum 값만큼 버튼 생성
	for (int32 i = 0; i < MatEnumPtr->NumEnums() - 1; ++i)
	{
		UButton* Button = WidgetTree->ConstructWidget<UButton>(UButton::StaticClass());

		//생성된 버튼들을 Array에 담기. 바깥에서 인덱스 하나하나에 접근해서 이벤트를 수동으로 걸어주기 위해.
		MatTypeBtnArray.Add(Button);
		UTextBlock* Text = WidgetTree->ConstructWidget<UTextBlock>(UTextBlock::StaticClass());
		
		FText EnumName = MatEnumPtr->GetDisplayNameTextByIndex(i);
		Text->SetText(EnumName);
		Button->AddChild(Text);

		UVerticalBoxSlot* BoxSlot = ButtonBox->AddChildToVerticalBox(Button);
		BoxSlot->SetPadding(FMargin(3.f));
		
		// 🔹 스타일 설정
		FButtonStyle ButtonStyle;
		ButtonStyle.Normal.TintColor = FSlateColor(FLinearColor::Gray);
		ButtonStyle.Hovered.TintColor = FSlateColor(FLinearColor(0.7f, 0.7f, 1.f, 1.f));
		ButtonStyle.Pressed.TintColor = FSlateColor(FLinearColor(0.5f, 0.5f, 1.f, 1.f));
		Button->SetStyle(ButtonStyle);
	}
	MaterialTypeChangePanel->SetVisibility(ESlateVisibility::Hidden);
}

void UMaterialGeneratePanel::OnTireBtnClicked()
{
	MaterialTypeText->SetText(FText::FromString(MatEnumPtr->GetNameStringByIndex(0)));
	matTypeSelected = EMaterialType::Tire;
	MaterialTypeChangePanel->SetVisibility(ESlateVisibility::Hidden);

}

void UMaterialGeneratePanel::OnFoliageBtnClicked()
{
	MaterialTypeText->SetText(FText::FromString(MatEnumPtr->GetNameStringByIndex(1)));
	matTypeSelected = EMaterialType::Foliage;
	MaterialTypeChangePanel->SetVisibility(ESlateVisibility::Hidden);

}

void UMaterialGeneratePanel::OnVideoBtnClicked()
{
	MaterialTypeText->SetText(FText::FromString(MatEnumPtr->GetNameStringByIndex(2)));
	matTypeSelected = EMaterialType::Video;
	MaterialTypeChangePanel->SetVisibility(ESlateVisibility::Hidden);

}

void UMaterialGeneratePanel::OnSubsurfaceBtnClicked()
{
	MaterialTypeText->SetText(FText::FromString(MatEnumPtr->GetNameStringByIndex(3)));
	matTypeSelected = EMaterialType::Subsurface;
	MaterialTypeChangePanel->SetVisibility(ESlateVisibility::Hidden);

}

void UMaterialGeneratePanel::OnCarPaintBtnClicked()
{
	MaterialTypeText->SetText(FText::FromString(MatEnumPtr->GetNameStringByIndex(4)));
	matTypeSelected = EMaterialType::CarPaint;
	MaterialTypeChangePanel->SetVisibility(ESlateVisibility::Hidden);

}

void UMaterialGeneratePanel::OnWaterBtnClicked()
{
	MaterialTypeText->SetText(FText::FromString(MatEnumPtr->GetNameStringByIndex(5)));
	matTypeSelected = EMaterialType::Water;
	MaterialTypeChangePanel->SetVisibility(ESlateVisibility::Hidden);

}

void UMaterialGeneratePanel::OnEmissiveBtnClicked()
{
	MaterialTypeText->SetText(FText::FromString(MatEnumPtr->GetNameStringByIndex(6)));
	matTypeSelected = EMaterialType::Emissive;
	MaterialTypeChangePanel->SetVisibility(ESlateVisibility::Hidden);

}

void UMaterialGeneratePanel::OnFabricBtnClicked()
{
	MaterialTypeText->SetText(FText::FromString(MatEnumPtr->GetNameStringByIndex(7)));
	matTypeSelected = EMaterialType::Fabric;
	MaterialTypeChangePanel->SetVisibility(ESlateVisibility::Hidden);

}

void UMaterialGeneratePanel::OnGlassBtnClicked()
{
	MaterialTypeText->SetText(FText::FromString(MatEnumPtr->GetNameStringByIndex(8)));
	matTypeSelected = EMaterialType::Glass;
	MaterialTypeChangePanel->SetVisibility(ESlateVisibility::Hidden);

}

void UMaterialGeneratePanel::OnStandardBtnClicked()
{
	MaterialTypeText->SetText(FText::FromString(MatEnumPtr->GetNameStringByIndex(9)));
	matTypeSelected = EMaterialType::Standard;
	MaterialTypeChangePanel->SetVisibility(ESlateVisibility::Hidden);

}

void UMaterialGeneratePanel::OnCreateMaterialBtnClicked()
{
	
	// 데이터 생성
	UMaterialThumbnailData* NewItem = NewObject<UMaterialThumbnailData>(this);
	const UEnum* MaterialEnum = StaticEnum<EMaterialType>();
	FText EnumDisplayNameText = MaterialEnum->GetDisplayNameTextByValue(
		static_cast<int64>(matTypeSelected)
	);
	FString EnumNameString = EnumDisplayNameText.ToString();
	FString NewMaterialName = FString::Printf(TEXT("New %s"), *EnumNameString);
	
	NewItem->MaterialName = NewMaterialName;
	NewItem->MaterialType = matTypeSelected;

	UE_LOG(LogTemp, Warning, TEXT("NewItem MaterialName: %s"), *NewItem->MaterialName);
	UE_LOG(LogTemp, Warning, TEXT("NewItem MaterialType: %d"), static_cast<int32>(NewItem->MaterialType));


	// 샘플 썸네일 (임시로 Content 폴더에서 불러오기)
	NewItem->Thumbnail = LoadObject<UTexture2D>(nullptr, TEXT("/Game/UI/Thumbnails/T_GlassIcon.T_GlassIcon"));

	// TileView에 추가
	MaterialTileView->AddItem(NewItem);
}

