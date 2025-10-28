// Fill out your copyright notice in the Description page of Project Settings.

#include "UI/Panel/BottomView.h"
#include "Components/WidgetSwitcher.h"

void UBottomView::SetBottomViewSwitcherIndex(int32 Index)
{
	if (!BottomViewSwitcher)
	{
		UE_LOG(LogTemp, Warning, TEXT("BottomView: BottomViewSwitcher is null"));
		return;
	}

	// WidgetSwitcher 인덱스 변경
	BottomViewSwitcher->SetActiveWidgetIndex(Index);

	UE_LOG(LogTemp, Log, TEXT("BottomView: BottomViewSwitcher set to index %d"), Index);
}

void UBottomView::SetBottomViewVisibility(bool bVisible)
{
	if (bVisible)
	{
		SetVisibility(ESlateVisibility::Visible);
		UE_LOG(LogTemp, Log, TEXT("BottomView: Panel shown"));
	}
	else
	{
		SetVisibility(ESlateVisibility::Collapsed);
		UE_LOG(LogTemp, Log, TEXT("BottomView: Panel hidden"));
	}
}
