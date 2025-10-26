// Fill out your copyright notice in the Description page of Project Settings.

#include "UI/Panel/BottomBar.h"
#include "UI/WidgetController/BottomController.h"
#include "UI/Panel/RightPanel.h"
#include "UI/Panel/LibraryPanel.h"
#include "Components/Button.h"

void UBottomBar::NativeConstruct()
{
	Super::NativeConstruct();

	// 초기 상태 설정
	bIsLibraryPanelVisible = false;

	// Bind button click events
	if (LibraryButton)
	{
		LibraryButton->OnClicked.AddDynamic(this, &UBottomBar::OnLibraryClicked);
	}

	if (SceneButton)
	{
		SceneButton->OnClicked.AddDynamic(this, &UBottomBar::OnSceneClicked);
	}

	if (PropertiesButton)
	{
		PropertiesButton->OnClicked.AddDynamic(this, &UBottomBar::OnPropertiesClicked);
	}

	if (UserListButton)
	{
		UserListButton->OnClicked.AddDynamic(this, &UBottomBar::OnUserListClicked);
	}

	UE_LOG(LogTemp, Log, TEXT("BottomBar constructed and buttons bound"));
}

void UBottomBar::InitializeWithRightPanel(URightPanel* InRightPanel)
{
	if (!InRightPanel)
	{
		UE_LOG(LogTemp, Warning, TEXT("BottomBar: RightPanel is null"));
		return;
	}

	// BottomController 생성 및 초기화
	if (!BottomController)
	{
		BottomController = NewObject<UBottomController>(this);
		BottomController->InitializeWithRightPanel(InRightPanel);

		UE_LOG(LogTemp, Log, TEXT("BottomBar: BottomController initialized"));
	}
}

void UBottomBar::InitializeWithLibraryPanel(ULibraryPanel* InLibraryPanel)
{
	if (!InLibraryPanel)
	{
		UE_LOG(LogTemp, Warning, TEXT("BottomBar: LibraryPanel is null"));
		return;
	}

	LibraryPanel = InLibraryPanel;

	// LibraryPanel 초기 상태: 숨김
	LibraryPanel->SetLibraryPanelVisibility(false);
	bIsLibraryPanelVisible = false;

	UE_LOG(LogTemp, Log, TEXT("BottomBar: LibraryPanel initialized (hidden)"));
}

void UBottomBar::OnLibraryClicked()
{
	OnLibraryButtonClicked.Broadcast();
	UE_LOG(LogTemp, Log, TEXT("BottomBar: Library button clicked"));

	// LibraryPanel 토글
	if (LibraryPanel)
	{
		bIsLibraryPanelVisible = !bIsLibraryPanelVisible;
		LibraryPanel->SetLibraryPanelVisibility(bIsLibraryPanelVisible);

		if (bIsLibraryPanelVisible)
		{
			UE_LOG(LogTemp, Log, TEXT("BottomBar: LibraryPanel shown"));
		}
		else
		{
			UE_LOG(LogTemp, Log, TEXT("BottomBar: LibraryPanel hidden"));
		}
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("BottomBar: LibraryPanel is null"));
	}
}

void UBottomBar::OnSceneClicked()
{
	OnSceneButtonClicked.Broadcast();
	UE_LOG(LogTemp, Log, TEXT("BottomBar: Scene button clicked"));

	// BottomController를 통해 RightPanel 토글
	if (BottomController)
	{
		BottomController->ToggleScenePanel();
	}
}

void UBottomBar::OnPropertiesClicked()
{
	OnPropertiesButtonClicked.Broadcast();
	UE_LOG(LogTemp, Log, TEXT("BottomBar: Properties button clicked"));

	// BottomController를 통해 RightPanel 토글
	if (BottomController)
	{
		BottomController->TogglePropertiesPanel();
	}
}

void UBottomBar::OnUserListClicked()
{
	OnUserListButtonClicked.Broadcast();
	UE_LOG(LogTemp, Log, TEXT("BottomBar: UserList button clicked"));

	// BottomController를 통해 RightPanel 토글 (index 3: UserPanel)
	if (BottomController)
	{
		UE_LOG(LogTemp, Log, TEXT("BottomBar: BottomController"));
		BottomController->ToggleUserListPanel();
	}
	if (!BottomController)
	{
		UE_LOG(LogTemp, Log, TEXT("BottomBar: UserList button clicked"));
		
	}

	
}
