
#include "UI/Panel/RightPanel.h"
#include "Engine/World.h"
#include "Engine/Level.h"
#include "Components/TreeView.h"
#include "Components/EditableTextBox.h"
#include "UI/WidgetController/SceneItemData.h"
#include "UI/Widget/SceneItemWidget.h"
#include "Engine/Light.h"
#include "Engine/DirectionalLight.h"
#include "Engine/PointLight.h"
#include "Engine/SpotLight.h"
#include "Camera/CameraActor.h"
#include "Engine/StaticMeshActor.h"
#include "EngineUtils.h"
#include "Components/TextBlock.h"
#include "Components/Button.h"
#include "Components/Image.h"

#if WITH_EDITOR
#include "Engine/Selection.h"
#include "Editor.h"
#endif

/**
 * MVC 패턴: Controller가 View 생성
 * Outliner처럼 각 아이템에 대한 Row Widget을 동적으로 생성
 */
UUserWidget* URightPanel::OnGenerateRow(UObject* Item)
{
    if (USceneItemData* ItemData = Cast<USceneItemData>(Item))
    {
        if (SceneItemWidgetClass)
        {
            // View 생성
            UUserWidget* Row = CreateWidget<UUserWidget>(GetWorld(), SceneItemWidgetClass);

            if (USceneItemWidget* SceneItemRow = Cast<USceneItemWidget>(Row))
            {
                // IUserObjectListEntry를 통해 자동으로 데이터 설정됨
                // NativeOnListItemObjectSet이 호출됨
                return SceneItemRow;
            }
            else
            {
                // Fallback: 일반 UserWidget인 경우 직접 설정
                if (UTextBlock* NameText = Cast<UTextBlock>(Row->GetWidgetFromName(TEXT("ItemLabel"))))
                {
                    NameText->SetText(FText::FromString(ItemData->DisplayName));
                }
                if (UTextBlock* TypeText = Cast<UTextBlock>(Row->GetWidgetFromName(TEXT("ItemType"))))
                {
                    TypeText->SetText(FText::FromString(ItemData->ActorType));
                }
                return Row;
            }
        }
    }
    return nullptr;
}

void URightPanel::NativeConstruct()
{
    Super::NativeConstruct();

    if (SceneList)
    {
        // TreeView 델리게이트 바인딩 (UE 5.6 API)
        SceneList->SetOnGetItemChildren(this, &URightPanel::OnGetItemChildren);

        // Blueprint 이벤트에 바인딩
        //SceneList->BP_OnItemExpansionChanged.AddDynamic(this, &URightPanel::OnItemExpansionChangedEvent);
        //SceneList->BP_OnItemSelectionChanged.AddDynamic(this, &URightPanel::OnItemSelectionChangedEvent);
    }

#if WITH_EDITOR
    // 에디터 선택 변경 이벤트 바인딩
    if (GEditor)
    {
        GEditor->GetSelectedActors()->SelectionChangedEvent.AddUObject(this, &URightPanel::OnEditorSelectionChanged);
    }
#endif

    RefreshSceneActors();

#if WITH_EDITOR
    SyncSelectionFromEditor();
#endif
}

void URightPanel::NativeDestruct()
{
#if WITH_EDITOR
    // 에디터 선택 변경 이벤트 언바인딩
    if (GEditor)
    {
        GEditor->GetSelectedActors()->SelectionChangedEvent.RemoveAll(this);
    }
#endif

    Super::NativeDestruct();
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

        // 언리얼 에디터 Outliner처럼 모든 액터 표시 (숨김 상태는 아이콘으로만 표시)
        // IsHidden()이나 IsEditorOnly() 체크 제거

        // 데이터 객체 생성
        USceneItemData* ItemData = NewObject<USceneItemData>(this);
        ItemData->Actor = Actor;
        ItemData->DisplayName = Actor->GetActorLabel();
        ItemData->bIsVisible = !Actor->IsHidden();

        // 액터 타입 설정 (사용자 친화적인 이름)
        if (Actor->IsA<ACameraActor>())
        {
            ItemData->ActorType = TEXT("Camera");
        }
        else if (Actor->IsA<ADirectionalLight>())
        {
            ItemData->ActorType = TEXT("Directional Light");
        }
        else if (Actor->IsA<APointLight>())
        {
            ItemData->ActorType = TEXT("Point Light");
        }
        else if (Actor->IsA<ASpotLight>())
        {
            ItemData->ActorType = TEXT("Spot Light");
        }
        else if (Actor->IsA<ALight>())
        {
            ItemData->ActorType = TEXT("Light");
        }
        else if (Actor->IsA<AStaticMeshActor>())
        {
            ItemData->ActorType = TEXT("Static Mesh");
        }
        else
        {
            // 클래스 이름에서 'A' 접두사 제거
            FString ClassName = Actor->GetClass()->GetName();
            if (ClassName.StartsWith(TEXT("A")))
            {
                ClassName.RightChopInline(1);
            }
            ItemData->ActorType = ClassName;
        }

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

    // TreeView 업데이트 - SceneList에 RootItems 설정
    if (SceneList)
    {
        //UE_LOG(LogTemp, Warning, TEXT("RefreshSceneActors: Found %d root items, %d total actors"), RootItems.Num(), ActorToItemMap.Num());

        SceneList->SetListItems(RootItems);

        // 데이터 확인 로그
        for (USceneItemData* Item : RootItems)
        {
            //UE_LOG(LogTemp, Log, TEXT("  Root Item: %s (%s)"), *Item->DisplayName, *Item->ActorType);
        }
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

void URightPanel::OnItemExpansionChangedEvent(UObject* Item, bool bIsExpanded)
{
    // 확장/축소 상태 저장 가능
}

void URightPanel::OnItemSelectionChangedEvent(UObject* Item, bool bIsSelected)
{
#if WITH_EDITOR
    if (bIsSelectingFromList || !GEditor || !bIsSelected)
        return;

    // SceneList에서 선택된 아이템들을 에디터에도 반영
    bIsSelectingFromList = true;

    TArray<UObject*> SelectedItems;
    SceneList->GetSelectedItems(SelectedItems);

    // 에디터 선택 클리어
    GEditor->SelectNone(false, true);

    // 선택된 아이템들의 액터를 에디터에서 선택
    for (UObject* SelectedItemObj : SelectedItems)
    {
        if (USceneItemData* ItemData = Cast<USceneItemData>(SelectedItemObj))
        {
            if (AActor* Actor = ItemData->Actor)
            {
                GEditor->SelectActor(Actor, true, false);
            }
        }
    }

    // 선택 변경 알림
    GEditor->NoteSelectionChange();

    bIsSelectingFromList = false;
#else
    // 런타임에서는 선택된 액터 정보만 저장 (향후 확장 가능)
    if (bIsSelected)
    {
        if (USceneItemData* ItemData = Cast<USceneItemData>(Item))
        {
            UE_LOG(LogTemp, Log, TEXT("Selected Actor: %s"), *ItemData->DisplayName);
        }
    }
#endif
}

#if WITH_EDITOR
void URightPanel::OnEditorSelectionChanged(UObject* NewSelection)
{
    if (!bIsSelectingFromList)
    {
        SyncSelectionFromEditor();
    }
}

void URightPanel::SyncSelectionFromEditor()
{
    if (!SceneList || !GEditor)
        return;

    // 에디터에서 선택된 액터들 가져오기
    USelection* SelectedActors = GEditor->GetSelectedActors();
    if (!SelectedActors)
        return;

    bIsSelectingFromList = true;

    // SceneList 선택 클리어
    SceneList->ClearSelection();

    // 선택된 액터들을 SceneList에서도 선택
    TArray<AActor*> Actors;
    SelectedActors->GetSelectedObjects<AActor>(Actors);

    for (AActor* Actor : Actors)
    {
        if (USceneItemData** ItemDataPtr = ActorToItemMap.Find(Actor))
        {
            SceneList->SetItemSelection(*ItemDataPtr, true);

            // 부모 아이템들을 확장하여 선택된 아이템이 보이도록 함
            AActor* ParentActor = Actor->GetAttachParentActor();
            while (ParentActor)
            {
                if (USceneItemData** ParentDataPtr = ActorToItemMap.Find(ParentActor))
                {
                    SceneList->SetItemExpansion(*ParentDataPtr, true);
                }
                ParentActor = ParentActor->GetAttachParentActor();
            }
        }
    }

    bIsSelectingFromList = false;
}
#endif
