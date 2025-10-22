// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/Panel/MaterialGeneratePanel.h"

#include "Blueprint/WidgetTree.h"
#include "Components/Border.h"
#include "Components/Button.h"
#include "Components/CanvasPanel.h"
#include "Components/CanvasPanelSlot.h"
#include "Components/TextBlock.h"
#include "Components/VerticalBox.h"
#include "Components/VerticalBoxSlot.h"

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

	// 3. 성공 반환
	return true;
}

void UMaterialGeneratePanel::CreateMaterialTypeEnumBtns()
{

	UEnum* EnumPtr = StaticEnum<EMaterialType>();


	// 1️⃣ Border 생성
	UBorder* BackgroundBorder = WidgetTree->ConstructWidget<UBorder>(UBorder::StaticClass());
	BackgroundBorder->SetBrushColor(FLinearColor::Black); // 검은색 배경
	BackgroundBorder->SetPadding(FMargin(5.f)); // 버튼과 여백

	// 2️⃣ VerticalBox 생성
	UVerticalBox* ButtonBox = WidgetTree->ConstructWidget<UVerticalBox>(UVerticalBox::StaticClass());

	// 3️⃣ Border 안에 VerticalBox 추가
	BackgroundBorder->AddChild(ButtonBox);

	// 4️⃣ Canvas에 Border 추가
	UCanvasPanelSlot* CanvasSlot = MaterialTypeCanvas->AddChildToCanvas(BackgroundBorder);
	CanvasSlot->SetAutoSize(true);
	CanvasSlot->SetPosition(FVector2D(20, -480));

	// 🔹 3️⃣ Enum 값만큼 버튼 생성
	for (int32 i = 0; i < EnumPtr->NumEnums() - 1; ++i)
	{
		UButton* Button = WidgetTree->ConstructWidget<UButton>(UButton::StaticClass());
		UTextBlock* Text = WidgetTree->ConstructWidget<UTextBlock>(UTextBlock::StaticClass());

		FText EnumName = EnumPtr->GetDisplayNameTextByIndex(i);
		Text->SetText(EnumName);
		Button->AddChild(Text);

		UVerticalBoxSlot* BoxSlot = ButtonBox->AddChildToVerticalBox(Button);
		BoxSlot->SetPadding(FMargin(3.f));

		EMaterialType Type = static_cast<EMaterialType>(i);
		Button->OnClicked.AddUniqueDynamic(this, &UMaterialGeneratePanel::OnMaterialTypeSelectionBtnClicked);

		FButtonStyle ButtonStyle;
		ButtonStyle.Normal.TintColor = FSlateColor(FLinearColor::Gray);
		ButtonStyle.Hovered.TintColor = FSlateColor(FLinearColor(0.7f, 0.7f, 1.f, 1.f)); // 호버 시 색상
		ButtonStyle.Pressed.TintColor = FSlateColor(FLinearColor(0.5f, 0.5f, 1.f, 1.f));
		Button->SetStyle(ButtonStyle);
	}
	
}

void UMaterialGeneratePanel::OnMaterialTypeSelectionBtnClicked()
{
}
