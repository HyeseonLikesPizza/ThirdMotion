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

	// 선택된 액터가 BP_Memo인지 확인
	if (!SelectedActor->GetName().Contains(TEXT("BP_Memo")))
	{
		UE_LOG(LogTemp, Warning, TEXT("MemoWidget: Selected actor is not BP_Memo"));
		return;
	}

	// 선택된 BP_Memo 액터의 WidgetComponent 찾기
	UWidgetComponent* WidgetComp = SelectedActor->FindComponentByClass<UWidgetComponent>();
	if (!WidgetComp)
	{
		UE_LOG(LogTemp, Warning, TEXT("MemoWidget: WidgetComponent not found in selected BP_Memo"));
		return;
	}

	// WidgetComponent에서 ViewMemo 위젯 가져오기
	UUserWidget* ViewMemo = WidgetComp->GetWidget();
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

	// RPC로 네트워크 동기화 (선택된 액터 정보 포함)
	FGuid ActorGuid;
	// BP_Memo가 EditSyncComponent를 가지고 있다면 Guid 사용 가능
	// 여기서는 Actor 이름으로 동기화
	PC->Server_UpdateMemoText(NewText.ToString());
	UE_LOG(LogTemp, Log, TEXT("MemoWidget: Sent memo update to server for selected BP_Memo"));
}
