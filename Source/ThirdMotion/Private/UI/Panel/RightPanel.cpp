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
	}

	// UI 새로고침
	RefreshUI();
}

void URightPanel::NativeDestruct()
{
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

		// SceneList 먼저 생성
		if (!SceneListData)
		{
			InitializeSceneList();
		}

		// Controller 초기화
		SceneController->Initialize(GetWorld());

		UE_LOG(LogTemp, Log, TEXT("RightPanel: SceneController initialized"));
	}
}

void URightPanel::InitializeSceneList()
{
	if (!SceneListData)
	{
		SceneListData = NewObject<USceneList>(this);
		SceneListData->Initialize(GetWorld());

		UE_LOG(LogTemp, Log, TEXT("RightPanel: SceneList initialized with %d items"),
			SceneListData->GetItemCount());
	}
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
	if (!bIsSelected || !SceneController || !Item)
		return;

	if (USceneItemData* ItemData = Cast<USceneItemData>(Item))
	{
		if (ItemData->Actor)
		{
			SceneController->SelectActor(ItemData->Actor);
			UE_LOG(LogTemp, Log, TEXT("RightPanel: Selected actor %s"), *ItemData->Actor->GetName());
		}
	}
}
