// Fill out your copyright notice in the Description page of Project Settings.

#include "UI/Widget/MemoWidget.h"
#include "Components/MultiLineEditableTextBox.h"
#include "Components/Button.h"
#include "Components/WidgetComponent.h"
#include "Framework/ThirdMotionPlayerController.h"
#include "Kismet/GameplayStatics.h"

void UMemoWidget::NativeConstruct()
{
	Super::NativeConstruct();

	// UpdateButton 클릭 이벤트 바인딩
	if (UpdateButton)
	{
		UpdateButton->OnClicked.AddDynamic(this, &UMemoWidget::OnUpdateButtonClicked);
	}

	UE_LOG(LogTemp, Log, TEXT("MemoWidget: NativeConstruct - UpdateButton bound"));
}

void UMemoWidget::NativeDestruct()
{
	// 이벤트 언바인딩
	if (UpdateButton)
	{
		UpdateButton->OnClicked.RemoveDynamic(this, &UMemoWidget::OnUpdateButtonClicked);
	}

	Super::NativeDestruct();
}

void UMemoWidget::OnUpdateButtonClicked()
{
	if (!NoteTextBox)
	{
		UE_LOG(LogTemp, Warning, TEXT("MemoWidget: NoteTextBox is null"));
		return;
	}

	// NoteTextBox의 텍스트 가져오기
	FText MemoText = NoteTextBox->GetText();

	UE_LOG(LogTemp, Log, TEXT("MemoWidget: UpdateButton clicked - Text: %s"), *MemoText.ToString());

	// 메모 텍스트 업데이트 (로컬 + RPC)
	UpdateMemoText(MemoText);
}

void UMemoWidget::UpdateMemoText(const FText& NewText)
{
	// PlayerController에서 선택된 액터 가져오기
	AThirdMotionPlayerController* PC = Cast<AThirdMotionPlayerController>(GetOwningPlayer());
	if (!PC)
	{
		UE_LOG(LogTemp, Warning, TEXT("MemoWidget: PlayerController is null"));
		return;
	}

	AActor* SelectedActor = PC->GetSelectedActor();
	if (!SelectedActor)
	{
		UE_LOG(LogTemp, Warning, TEXT("MemoWidget: No actor selected"));
		return;
	}

	// 선택된 액터의 WidgetComponent 중 태그가 "Memo"인 컴포넌트 찾기
	TArray<UWidgetComponent*> WidgetComponents;
	SelectedActor->GetComponents<UWidgetComponent>(WidgetComponents);

	UWidgetComponent* MemoWidgetComp = nullptr;
	for (UWidgetComponent* WidgetComp : WidgetComponents)
	{
		if (WidgetComp && WidgetComp->ComponentHasTag(TEXT("Memo")))
		{
			MemoWidgetComp = WidgetComp;
			UE_LOG(LogTemp, Log, TEXT("MemoWidget: Found component with 'Memo' tag - Component: %s"), *WidgetComp->GetName());
			break;
		}
	}

	if (!MemoWidgetComp)
	{
		UE_LOG(LogTemp, Warning, TEXT("MemoWidget: Component with 'Memo' tag not found in selected actor"));
		return;
	}

	// MemoWidgetComponent에서 ViewMemo 위젯 가져오기
	UUserWidget* ViewMemo = MemoWidgetComp->GetWidget();
	if (!ViewMemo)
	{
		UE_LOG(LogTemp, Warning, TEXT("MemoWidget: ViewMemo widget is null"));
		return;
	}

	// ViewMemo의 NoteTextBox 찾기
	UMultiLineEditableTextBox* ViewNoteTextBox = Cast<UMultiLineEditableTextBox>(
		ViewMemo->GetWidgetFromName(TEXT("NoteTextBox")));

	if (ViewNoteTextBox)
	{
		ViewNoteTextBox->SetText(NewText);
		UE_LOG(LogTemp, Log, TEXT("MemoWidget: Updated selected BP_Memo NoteTextBox - Text: %s"),
			*NewText.ToString());
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("MemoWidget: NoteTextBox not found in ViewMemo"));
	}

	UE_LOG(LogTemp, Log, TEXT("MemoWidget: Memo text updated successfully"));
}
