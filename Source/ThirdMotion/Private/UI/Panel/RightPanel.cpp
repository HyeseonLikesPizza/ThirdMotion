#include "UI/Panel/RightPanel.h"
#include "UI/Widget/SceneItemWidget.h"
#include "Data/SceneItemData.h"
#include "UI/WidgetController/SceneController.h"
#include "Data/SceneList.h"
#include "Components/TreeView.h"
#include "Components/TextBlock.h"
#include "Engine/World.h"


//if WITH_EDITOR: 에디터 전용 코드를 구분하는 전처리 지시자
#if WITH_EDITOR
// 에디터에서 실행 
#include "Engine/Selection.h"
#include "Editor.h"
#endif

/**
 * Row Widget 생성 - View만 담당
 */
UUserWidget* URightPanel::OnGenerateRow(UObject* Item)
{
	if (USceneItemData* ItemData = Cast<USceneItemData>(Item))
	{
		if (SceneItemWidgetClass)
		{
			// SceneItemWidget 생성
			UUserWidget* Row = CreateWidget<UUserWidget>(GetWorld(), SceneItemWidgetClass);

			if (USceneItemWidget* SceneItemRow = Cast<USceneItemWidget>(Row))
			{
				// SceneController 전달
				SceneItemRow->SetSceneController(SceneController);
				return SceneItemRow;
			}

			return Row;
		}
	}
	return nullptr;
}

void URightPanel::NativeConstruct()
{
	Super::NativeConstruct();

	// Controller 초기화
	InitializeController();

	// SceneList 초기화
	InitializeSceneList();

	// TreeView 설정
	if (SceneList)
	{
		SceneList->SetOnGetItemChildren(this, &URightPanel::OnGetItemChildren);
		// SceneList->BP_OnItemSelectionChanged.AddDynamic(this, &URightPanel::OnItemSelectionChangedEvent);
	}

#if WITH_EDITOR
	// 에디터 선택 변경 이벤트 바인딩
	if (GEditor)
	{
		GEditor->GetSelectedActors()->SelectionChangedEvent.AddUObject(this, &URightPanel::OnEditorSelectionChanged);
	}
#endif

	// UI 새로고침
	RefreshUI();

#if WITH_EDITOR
	SyncSelectionFromEditor();
#endif
}

void URightPanel::NativeDestruct()
{
#if WITH_EDITOR
	if (GEditor)
	{
		GEditor->GetSelectedActors()->SelectionChangedEvent.RemoveAll(this);
	}
#endif

	// 델리게이트 해제
	if (SceneListData)
	{
		SceneListData->OnDataChanged.RemoveDynamic(this, &URightPanel::OnSceneDataChanged);
	}

	if (SceneController)
	{
		SceneController->OnSelectionChanged.RemoveDynamic(this, &URightPanel::OnSelectionChanged);
		SceneController->OnSceneChanged.RemoveDynamic(this, &URightPanel::OnSceneDataChanged);
	}

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

		if (SceneListData)
		{
			SceneListData->RefreshFromWorld();
		}
	}
}

void URightPanel::InitializeController()
{
	if (!SceneController)
	{
		SceneController = NewObject<USceneController>(this);
		SceneController->Initialize(GetWorld());

		// 델리게이트 바인딩
		SceneController->OnSelectionChanged.AddDynamic(this, &URightPanel::OnSelectionChanged);
		SceneController->OnSceneChanged.AddDynamic(this, &URightPanel::OnSceneDataChanged);

		UE_LOG(LogTemp, Log, TEXT("RightPanel: SceneController initialized"));
	}
}

void URightPanel::InitializeSceneList()
{
	if (!SceneListData)
	{
		SceneListData = NewObject<USceneList>(this);
		SceneListData->Initialize(GetWorld());

		// 델리게이트 바인딩
		SceneListData->OnDataChanged.AddDynamic(this, &URightPanel::OnSceneDataChanged);

		UE_LOG(LogTemp, Log, TEXT("RightPanel: SceneList initialized with %d items"),
			SceneListData->GetItemCount());
	}
}

void URightPanel::OnSceneDataChanged()
{
	RefreshUI();
}

void URightPanel::OnSelectionChanged(const TArray<AActor*>& SelectedActors)
{
	if (!SceneList || !SceneListData) return;

	// 모든 아이템의 선택 상태 초기화
	TArray<USceneItemData*> AllItems = SceneListData->GetAllItems();
	for (USceneItemData* Item : AllItems)
	{
		if (Item)
		{
			Item->bIsSelected = false;
			Item->NotifyDataChanged(); // Observer에게 알림
		}
	}

	// TreeView 선택 업데이트
	SceneList->ClearSelection();

	// 선택된 액터들의 ItemData 업데이트
	for (AActor* Actor : SelectedActors)
	{
		if (USceneItemData* ItemData = SceneListData->FindItemByActor(Actor))
		{
			ItemData->bIsSelected = true;
			ItemData->NotifyDataChanged(); // Observer에게 알림
			SceneList->SetItemSelection(ItemData, true);
		}
	}

	UE_LOG(LogTemp, Log, TEXT("RightPanel: Selection changed to %d actors"), SelectedActors.Num());
}

void URightPanel::RefreshUI()
{
	if (!SceneList || !SceneListData) return;

	// TreeView에 데이터 설정
	TArray<USceneItemData*> RootItems = SceneListData->GetRootItems();
	SceneList->SetListItems(RootItems);

	UE_LOG(LogTemp, Log, TEXT("RightPanel: UI refreshed with %d root items"), RootItems.Num());
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

void URightPanel::OnItemSelectionChangedEvent(UObject* Item, bool bIsSelected)
{
#if WITH_EDITOR
	if (bIsSelectingFromList || !GEditor || !bIsSelected || !SceneController)
		return;

	bIsSelectingFromList = true;

	TArray<UObject*> SelectedItems;
	SceneList->GetSelectedItems(SelectedItems);

	TArray<AActor*> ActorsToSelect;
	for (UObject* SelectedItemObj : SelectedItems)
	{
		if (USceneItemData* ItemData = Cast<USceneItemData>(SelectedItemObj))
		{
			if (ItemData->Actor)
			{
				ActorsToSelect.Add(ItemData->Actor);
			}
		}
	}

	// SceneController를 통해 선택
	SceneController->SelectActors(ActorsToSelect);

	// 에디터 선택도 업데이트
	GEditor->SelectNone(false, true);
	for (AActor* Actor : ActorsToSelect)
	{
		GEditor->SelectActor(Actor, true, false);
	}
	GEditor->NoteSelectionChange();

	bIsSelectingFromList = false;
#else
	// 런타임 실행
	if (bIsSelected && SceneController)
	{
		if (USceneItemData* ItemData = Cast<USceneItemData>(Item))
		{
			if (ItemData->Actor)
			{
				SceneController->SelectActor(ItemData->Actor);
			}
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
	if (!SceneList || !GEditor || !SceneListData || !SceneController)
		return;

	USelection* SelectedActors = GEditor->GetSelectedActors();
	if (!SelectedActors)
		return;

	bIsSelectingFromList = true;

	// 선택된 액터들 가져오기
	TArray<AActor*> Actors;
	SelectedActors->GetSelectedObjects<AActor>(Actors);

	// SceneController에 선택 전달
	SceneController->SelectActors(Actors);

	// TreeView 선택 업데이트
	SceneList->ClearSelection();

	for (AActor* Actor : Actors)
	{
		if (USceneItemData* ItemData = SceneListData->FindItemByActor(Actor))
		{
			SceneList->SetItemSelection(ItemData, true);

			// 부모 아이템들을 확장하여 선택된 아이템이 보이도록 함
			AActor* ParentActor = Actor->GetAttachParentActor();
			while (ParentActor)
			{
				if (USceneItemData* ParentData = SceneListData->FindItemByActor(ParentActor))
				{
					SceneList->SetItemExpansion(ParentData, true);
				}
				ParentActor = ParentActor->GetAttachParentActor();
			}
		}
	}

	bIsSelectingFromList = false;
}
#endif
