// Fill out your copyright notice in the Description page of Project Settings.

#include "UI/Widget/MemoWidget.h"
#include "Components/MultiLineEditableTextBox.h"
#include "Components/Button.h"
#include "Components/WidgetComponent.h"
#include "Framework/ThirdMotionPlayerController.h"
#include "Edit/EditSyncComponent.h"
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

	// EditSyncComponent 가져오기
	UEditSyncComponent* EditSync = SelectedActor->FindComponentByClass<UEditSyncComponent>();
	if (!EditSync)
	{
		UE_LOG(LogTemp, Warning, TEXT("MemoWidget: EditSyncComponent not found"));
		return;
	}

	// 서버 권한 체크
	bool bIsServer = SelectedActor->HasAuthority();
	FString NewTextString = NewText.ToString();

	if (bIsServer)
	{
		// 서버인 경우: 직접 적용
		EditSync->ApplyMemoTextAuthoritative(NewTextString);
		UE_LOG(LogTemp, Log, TEXT("MemoWidget: Memo text applied directly (Server)"));
	}
	else
	{
		// 클라이언트인 경우: RPC 호출
		EditSync->Server_SetMemoText(NewTextString);
		UE_LOG(LogTemp, Log, TEXT("MemoWidget: Memo text RPC called (Client -> Server)"));
	}

	UE_LOG(LogTemp, Log, TEXT("MemoWidget: Memo text update requested - Text: %s"), *NewTextString);
}
