// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BaseWidget.h"
#include "MemoWidget.generated.h"

class UMultiLineEditableTextBox;
class UButton;

/**
 * MemoWidget - 메모 작성 위젯
 * NoteTextBox에 내용 작성 후 UpdateButton 클릭 시 선택된 BP_Memo의 ViewMemo에 동기화
 */
UCLASS()
class THIRDMOTION_API UMemoWidget : public UBaseWidget
{
	GENERATED_BODY()

public:
	virtual void NativeConstruct() override;
	virtual void NativeDestruct() override;

	// NoteTextBox (메모 입력)
	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	UMultiLineEditableTextBox* NoteTextBox;

	// UpdateButton (업데이트 버튼)
	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	UButton* UpdateButton;

protected:
	// UpdateButton 클릭 핸들러
	UFUNCTION()
	void OnUpdateButtonClicked();

	// 메모 텍스트 업데이트 (선택된 BP_Memo만)
	void UpdateMemoText(const FText& NewText);
};
