#include "UI/Panel/RightPanel.h"
#include "UI/Widget/SceneItemWidget.h"
#include "Data/SceneItemData.h"
#include "UI/WidgetController/RightPanelController.h"
#include "UI/WidgetController/SceneController.h"
#include "Data/SceneList.h"
#include "Edit/SceneManager.h"
#include "Components/TreeView.h"
#include "Components/TextBlock.h"
#include "Components/WidgetSwitcher.h"
#include "Engine/World.h"
#include "Framework/Application/SlateApplication.h"
#include "Network/ServerController.h"
#include "Engine/GameInstance.h"


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

	// SceneController 초기화 (Scene 패널용)
	InitializeSceneController();

	// SceneList 초기화 (Scene 패널용)
	InitializeSceneList();

	// SceneList 데이터 변경 이벤트 구독
	if (SceneListData)
	{
		SceneListData->OnDataChanged.AddDynamic(this, &URightPanel::OnSceneListDataChanged);
	}

	// SceneController의 선택 변경 이벤트 구독
	if (SceneController)
	{
		SceneController->OnSelectionChanged.AddDynamic(this, &URightPanel::OnActorSelectedInWorld);
	}

	// TreeView 설정 (Scene 패널용)
	if (SceneList)
	{
		SceneList->SetOnGetItemChildren(this, &URightPanel::OnGetItemChildren);
	}

	// RightPanelController 초기화
	InitializeRightPanelController();

	// UI 새로고침
	RefreshUI();
}

void URightPanel::NativeDestruct()
{
	// SceneList 데이터 변경 이벤트 구독 해제
	if (SceneListData)
	{
		SceneListData->OnDataChanged.RemoveDynamic(this, &URightPanel::OnSceneListDataChanged);
	}

	// SceneController 선택 변경 이벤트 구독 해제
	if (SceneController)
	{
		SceneController->OnSelectionChanged.RemoveDynamic(this, &URightPanel::OnActorSelectedInWorld);
	}

	Super::NativeDestruct();
}

void URightPanel::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);

	// 자동 업데이트로 대체됨 - NativeTick에서 RefreshFromWorld 호출 불필요
}

void URightPanel::InitializeRightPanelController()
{
	if (!RightPanelController)
	{
		RightPanelController = NewObject<URightPanelController>(this);
		RightPanelController->InitializeWithRightPanel(this);

		UE_LOG(LogTemp, Log, TEXT("RightPanel: RightPanelController initialized"));
	}
}

void URightPanel::InitializeSceneController()
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

		// SceneManager에 SceneList 등록 (자동 업데이트용)
		if (USceneManager* SceneMgr = GetWorld()->GetSubsystem<USceneManager>())
		{
			SceneMgr->RegisterSceneList(SceneListData);
		}

		// ServerController에도 SceneList 등록 (OnRep_Meta 처리용)
		if (UGameInstance* GameInstance = GetWorld()->GetGameInstance())
		{
			if (UServerController* ServerCtrl = GameInstance->GetSubsystem<UServerController>())
			{
				ServerCtrl->RegisterSceneList(GetWorld(), SceneListData);
			}
		}

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

void URightPanel::OnActorSelectedInWorld(const TArray<AActor*>& SelectedActors)
{
	if (SelectedActors.Num() == 0 || !SceneListData || !SceneList)
		return;

	// 첫 번째 Actor 선택
	AActor* SelectedActor = SelectedActors[0];
	if (!SelectedActor)
		return;

	// SceneList에서 해당 Actor의 ItemData 찾기
	USceneItemData* ItemData = SceneListData->FindItemByActor(SelectedActor);
	if (ItemData)
	{
		// TreeView에서 해당 항목 선택
		SceneList->SetSelectedItem(ItemData);
		UE_LOG(LogTemp, Log, TEXT("RightPanel: TreeView item selected for actor %s"),
			*SelectedActor->GetName());
	}
}

void URightPanel::SetWidgetSwitcherIndex(int32 Index)
{
	if (!WidgetSwitcher_Right)
	{
		UE_LOG(LogTemp, Warning, TEXT("RightPanel View: WidgetSwitcher_Right is null"));
		return;
	}

	// WidgetSwitcher 인덱스 변경 (View 기능만)
	WidgetSwitcher_Right->SetActiveWidgetIndex(Index);

	UE_LOG(LogTemp, Log, TEXT("RightPanel View: WidgetSwitcher set to index %d"), Index);
}

void URightPanel::SetRightPanelVisibility(bool bVisible)
{
	if (bVisible)
	{
		SetVisibility(ESlateVisibility::Visible);
		UE_LOG(LogTemp, Log, TEXT("RightPanel View: Panel shown"));
	}
	else
	{
		SetVisibility(ESlateVisibility::Collapsed);
		UE_LOG(LogTemp, Log, TEXT("RightPanel View: Panel hidden"));
	}
}

void URightPanel::OnSceneListDataChanged()
{
	// SceneList 데이터가 변경되면 UI 새로고침
	RefreshUI();
}

FReply URightPanel::NativeOnKeyDown(const FGeometry& InGeometry, const FKeyEvent& InKeyEvent)
{
	// Del 키 처리
	if (InKeyEvent.GetKey() == EKeys::Delete)
	{
		if (SceneController && SceneList)
		{
			// 선택된 항목 가져오기
			TArray<UObject*> SelectedItems;
			int32 NumSelected = SceneList->GetSelectedItems(SelectedItems);
			if (NumSelected > 0)
			{
				// 선택된 Actor들 삭제
				for (UObject* Item : SelectedItems)
				{
					if (USceneItemData* ItemData = Cast<USceneItemData>(Item))
					{
						if (ItemData->Actor)
						{
							UE_LOG(LogTemp, Warning, TEXT("RightPanel: Deleting actor - %s"),
								*ItemData->DisplayName);

							SceneController->DeleteActor(ItemData->Actor);
						}
					}
				}

				return FReply::Handled();
			}
		}
	}

	return Super::NativeOnKeyDown(InGeometry, InKeyEvent);
}
