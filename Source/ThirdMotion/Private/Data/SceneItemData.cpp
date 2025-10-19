#include "Data/SceneItemData.h"
#include "GameFramework/Actor.h"

void USceneItemData::UpdateFromActor()
{
	if (!Actor) return;

	// 액터로부터 데이터 업데이트
	DisplayName = Actor->GetActorLabel();
	bIsVisible = !Actor->IsHidden();

	// Observer 패턴: 변경 알림
	NotifyDataChanged();
}

void USceneItemData::NotifyDataChanged()
{
	// Observer들에게 데이터 변경 알림
	OnDataChanged.Broadcast(this);
}
