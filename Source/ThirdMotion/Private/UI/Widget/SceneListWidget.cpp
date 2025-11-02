// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/Widget/SceneListWidget.h"

#include "Components/TreeView.h"
#include "Data/SceneItemData.h"
#include "Data/SceneList.h"
#include "UI/Panel/RightPanel.h"
#include "UI/Widget/SceneItemWidget.h"
#include "UI/WidgetController/SceneController.h"


void USceneListWidget::RefreshUI()
{
	if (!SceneList || !SceneListData) return;

	// SceneListData의 모든 ItemData를 Actor 상태로부터 업데이트
	SceneListData->UpdateAllItems();

	// TreeView에 데이터 설정
	TArray<USceneItemData*> RootItems = SceneListData->GetRootItems();
	SceneList->SetListItems(RootItems);

	UE_LOG(LogTemp, Log, TEXT("SceneListWidget: UI refreshed with %d root items"), RootItems.Num());
}

UUserWidget* USceneListWidget::OnGenerateRow(UObject* Item)
{
	// 이 함수는 더 이상 사용되지 않음
	// TreeView의 EntryWidgetClass는 Blueprint에서 설정됨
	// SceneItemWidget이 IUserObjectListEntry를 구현하므로 자동으로 처리됨
	return nullptr;
}

void USceneListWidget::NativeConstruct()
{
	Super::NativeConstruct();

	// SceneController 초기화 (Scene 패널용)
	InitializeSceneController();

	// SceneList 초기화 (Scene 패널용)
	InitializeSceneList();

	// SceneList 데이터 변경 이벤트 구독
	if (SceneListData)
	{
		SceneListData->OnDataChanged.AddDynamic(this, &USceneListWidget::OnSceneListDataChanged);
	}

	// SceneController의 선택 변경 이벤트 구독
	if (SceneController)
	{
		SceneController->OnSelectionChanged.AddDynamic(this, &USceneListWidget::OnActorSelectedInWorld);
		SceneController->OnSceneChanged.AddDynamic(this, &USceneListWidget::OnSceneListDataChanged);
	}

	// TreeView 설정 (Scene 패널용)
	if (SceneList)
	{
		// EntryWidgetClass는 Blueprint에서 설정
		// OnGetItemChildren으로 자식 항목 제공
		SceneList->SetOnGetItemChildren(this, &USceneListWidget::OnGetItemChildren);

		UE_LOG(LogTemp, Log, TEXT("SceneListWidget: TreeView OnGetItemChildren delegate bound"));
	}

	// UI 새로고침
	RefreshUI();
}

void USceneListWidget::NativeDestruct()
{
	// SceneList 데이터 변경 이벤트 구독 해제
	if (SceneListData)
	{
		SceneListData->OnDataChanged.RemoveDynamic(this, &USceneListWidget::OnSceneListDataChanged);
	}

	// SceneController 선택 변경 이벤트 구독 해제
	if (SceneController)
	{
		SceneController->OnSelectionChanged.RemoveDynamic(this, &USceneListWidget::OnActorSelectedInWorld);
		SceneController->OnSceneChanged.RemoveDynamic(this, &USceneListWidget::OnSceneListDataChanged);
	}

	Super::NativeDestruct();
}

void USceneListWidget::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);

	// Play 중 Actor 상태 주기적 갱신 (0.5초마다)
	RefreshTimer += InDeltaTime;
	if (RefreshTimer >= 0.5f)
	{
		RefreshTimer = 0.0f;
		if (SceneListData && GetWorld())
		{
			// 기존 아이템들의 상태만 업데이트 (가시성, 이름 등)
			SceneListData->UpdateAllItems();
		}
	}
}

void USceneListWidget::InitializeSceneController()
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

void USceneListWidget::InitializeSceneList()
{
	if (!SceneListData)
	{
		SceneListData = NewObject<USceneList>(this);
		SceneListData->Initialize(GetWorld());

		// Play 중 GetWorld()로 Actor 목록 가져오기
		SceneListData->RefreshFromWorld();

		UE_LOG(LogTemp, Log, TEXT("SceneListWidget: SceneList initialized with %d items"),
			SceneListData->GetItemCount());
	}
}


void USceneListWidget::OnGetItemChildren(UObject* Item, TArray<UObject*>& Children)
{
	if (USceneItemData* ItemData = Cast<USceneItemData>(Item))
	{
		for (USceneItemData* Child : ItemData->Children)
		{
			Children.Add(Child);
		}
	}
}

void USceneListWidget::OnItemSelectionChangedEvent(UObject* Item, bool bIsSelected)
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

void USceneListWidget::OnActorSelectedInWorld(const TArray<AActor*>& SelectedActors)
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

void USceneListWidget::OnSceneListDataChanged()
{
	// SceneList 데이터가 변경되면 UI 새로고침
	RefreshUI();
}

FReply USceneListWidget::NativeOnKeyDown(const FGeometry& InGeometry, const FKeyEvent& InKeyEvent)
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
