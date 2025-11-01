// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/Widget/ChatWidget.h"

#include "Components/TextBlock.h"

void UChatWidget::SetContent(FString content)
{
	textContent->SetText(FText::FromString(content));
} 
