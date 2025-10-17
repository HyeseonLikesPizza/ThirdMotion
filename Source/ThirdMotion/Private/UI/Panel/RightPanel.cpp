
#include "UI/Panel/RightPanel.h"
#include "Engine/World.h"
#include "Engine/Level.h"
#include "Components/TreeView.h"
#include "Components/EditableTextBox.h"
#include "UI/WidgetController/SceneItemData.h"

void URightPanel::NativeConstruct()
{
    Super::NativeConstruct();

    if (SceneTreeView)
    {
        // TreeView 델리게이트 바인딩
       // SceneTreeView->OnGetItemChildren().AddUObject(this, &URightPanel::OnGetItemChildren);
        SceneTreeView->OnItemExpansionChanged().AddUObject(this, &URightPanel::OnItemExpansionChanged);
        
        // Entry Widget 클래스 설정 (블루프린트에서 설정하는 것이 좋음)
        // SceneTreeView->SetEntryWidgetClass(USceneTreeEntry::StaticClass());
    }

    RefreshSceneActors();
}

void URightPanel::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
    Super::NativeTick(MyGeometry, InDeltaTime);

    // 주기적으로 씬 업데이트 (0.5초마다)
    RefreshTimer += InDeltaTime;
    if (RefreshTimer > 0.5f)
    {
        RefreshTimer = 0.0f;
        RefreshSceneActors();
    }
}

void URightPanel::RefreshSceneActors()
{
    if (!GetWorld()) return;

    // 기존 데이터 클리어
    RootItems.Empty();
    ActorToItemMap.Empty();

    // 월드의 모든 액터 가져오기
    for (TActorIterator<AActor> It(GetWorld()); It; ++It)
    {
        AActor* Actor = *It;
        if (!Actor) continue;

        // 숨겨진 액터나 에디터 전용 액터 제외
        if (Actor->IsHidden() || Actor->IsEditorOnly()) continue;

        // 데이터 객체 생성
        USceneItemData* ItemData = NewObject<USceneItemData>(this);
        ItemData->Actor = Actor;
        ItemData->DisplayName = Actor->GetActorLabel();
        ItemData->bIsVisible = Actor->IsHidden() == false;

        // 아이콘 설정 (액터 타입에 따라)
        if (Actor->IsA<ALight>())
        {
            // Light 아이콘
        }
        else if (Actor->IsA<ACameraActor>())
        {
            // Camera 아이콘
        }
        // ... 기타 타입별 아이콘

        ActorToItemMap.Add(Actor, ItemData);

        // 부모가 없으면 루트 아이템
        if (!Actor->GetAttachParentActor())
        {
            RootItems.Add(ItemData);
        }
    }

    // 자식 관계 설정
    for (auto& Pair : ActorToItemMap)
    {
        AActor* Actor = Pair.Key;
        USceneItemData* ItemData = Pair.Value;

        if (AActor* ParentActor = Actor->GetAttachParentActor())
        {
            if (USceneItemData** ParentData = ActorToItemMap.Find(ParentActor))
            {
                (*ParentData)->Children.Add(ItemData);
            }
        }
    }

    // TreeView 업데이트
    if (SceneTreeView)
    {
        SceneTreeView->SetListItems(RootItems);
    }
}

void URightPanel::OnGetItemChildren(UObject* Item, TArray<UObject*>& Children)
{
    if (USceneItemData* ItemData = Cast<USceneItemData>(Item))
    {
        for (USceneItemData* Child : ItemData->Children)
        {
            Children.Add(Child);
        }
    }
}

void URightPanel::OnItemExpansionChanged(UObject* Item, bool bIsExpanded)
{
    // 확장/축소 상태 저장 가능
}

void URightPanel::OnItemSelectionChanged(UObject* Item)
{
    if (USceneItemData* ItemData = Cast<USceneItemData>(Item))
    {
        if (AActor* Actor = ItemData->Actor)
        {
            // 선택된 액터 처리
            // 예: 에디터에서 선택 표시, 프로퍼티 패널 업데이트 등
        }
    }
}