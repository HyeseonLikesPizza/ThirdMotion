// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/Panel/MaterialDetailsPanel.h"

#include "Components/TextBlock.h"

void UMaterialDetailsPanel::SetMaterialNameAndImage(FText name, UMaterial* mat)
{
	MaterialNameText->SetText(name);
	
}
