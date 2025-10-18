// Fill out your copyright notice in the Description page of Project Settings.

#include "UI/WidgetController/SceneItemData.h"
#include "GameFramework/Actor.h"

void USceneItemData::UpdateFromActor()
{
	if (!Actor)
	{
		return;
	}

	// 액터로부터 데이터 업데이트
	DisplayName = Actor->GetActorLabel();
	bIsVisible = !Actor->IsHidden();

	// Observer 패턴: 변경 알림
	NotifyDataChanged();
}

void USceneItemData::ToggleVisibility()
{
	if (!Actor)
	{
		return;
	}

	// 액터 가시성 토글
	bIsVisible = !bIsVisible;
	Actor->SetActorHiddenInGame(!bIsVisible);

#if WITH_EDITOR
	Actor->SetIsTemporarilyHiddenInEditor(!bIsVisible);
#endif

	// Observer 패턴: 변경 알림
	NotifyDataChanged();
}

void USceneItemData::SetDisplayName(const FString& NewName)
{
	if (!Actor)
	{
		return;
	}

	DisplayName = NewName;
	Actor->SetActorLabel(NewName);

	// Observer 패턴: 변경 알림
	NotifyDataChanged();
}

void USceneItemData::NotifyDataChanged()
{
	// Observer들에게 데이터 변경 알림
	OnDataChanged.Broadcast(this);
}
