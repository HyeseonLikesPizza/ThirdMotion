// Fill out your copyright notice in the Description page of Project Settings.

#include "UI/Widget/MemoWidget.h"
#include "Components/MultiLineEditableTextBox.h"
#include "Components/Button.h"
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
	// ViewMemoWidget의 NoteTextBox 업데이트
	if (ViewMemoWidget)
	{
		// ViewMemoWidget에서 NoteTextBox 찾기
		UMultiLineEditableTextBox* ViewNoteTextBox = Cast<UMultiLineEditableTextBox>(
			ViewMemoWidget->GetWidgetFromName(TEXT("NoteTextBox")));

		if (ViewNoteTextBox)
		{
			ViewNoteTextBox->SetText(NewText);
			UE_LOG(LogTemp, Log, TEXT("MemoWidget: Updated ViewMemo NoteTextBox - Text: %s"),
				*NewText.ToString());
		}
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("MemoWidget: ViewMemo NoteTextBox not found"));
		}
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("MemoWidget: ViewMemoWidget is null"));
	}

	// RPC로 네트워크 동기화
	if (AThirdMotionPlayerController* PC = Cast<AThirdMotionPlayerController>(GetOwningPlayer()))
	{
		PC->Server_UpdateMemoText(NewText.ToString());
		UE_LOG(LogTemp, Log, TEXT("MemoWidget: Sent memo update to server"));
	}
}
