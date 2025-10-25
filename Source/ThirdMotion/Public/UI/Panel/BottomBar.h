// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UI/Widget/BaseWidget.h"
#include "Components/Button.h"
#include "BottomBar.generated.h"

class UBottomController;
class URightPanel;

/**
 * Bottom Bar - Contains buttons for switching panels
 */
UCLASS()
class THIRDMOTION_API UBottomBar : public UBaseWidget
{
	GENERATED_BODY()

public:
	virtual void NativeConstruct() override;

	// BottomController 초기화 (RightPanel 참조 필요)
	UFUNCTION(BlueprintCallable, Category = "Bottom Bar")
	void InitializeWithRightPanel(URightPanel* InRightPanel);

	// BottomController 접근자
	UFUNCTION(BlueprintCallable, Category = "Bottom Bar")
	UBottomController* GetBottomController() const { return BottomController; }

	// Widget Components
	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	UButton* LibraryButton;

	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	UButton* SceneButton;

	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	UButton* PropertiesButton;

	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	UButton* UserListButton;

	// Events for panel switching
	DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnLibraryButtonClicked);
	DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnSceneButtonClicked);
	DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnPropertiesButtonClicked);
	DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnUserListButtonClicked);

	UPROPERTY(BlueprintAssignable, Category = "BottomBar Events")
	FOnLibraryButtonClicked OnLibraryButtonClicked;

	UPROPERTY(BlueprintAssignable, Category = "BottomBar Events")
	FOnSceneButtonClicked OnSceneButtonClicked;

	UPROPERTY(BlueprintAssignable, Category = "BottomBar Events")
	FOnPropertiesButtonClicked OnPropertiesButtonClicked;

	UPROPERTY(BlueprintAssignable, Category = "BottomBar Events")
	FOnUserListButtonClicked OnUserListButtonClicked;

protected:
	// Button click handlers
	UFUNCTION()
	void OnLibraryClicked();

	UFUNCTION()
	void OnSceneClicked();

	UFUNCTION()
	void OnPropertiesClicked();

	UFUNCTION()
	void OnUserListClicked();

private:
	// BottomController (패널 전환 로직 관리)
	UPROPERTY()
	UBottomController* BottomController;
};
