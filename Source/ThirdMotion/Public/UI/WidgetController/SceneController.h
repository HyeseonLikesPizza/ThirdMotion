#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "SceneController.generated.h"

class AActor;
class USceneItemData;

/**
 * SceneController - Controller 역할
 * 모든 Outline 기능(선택, 삭제, 가시성 등)을 중앙에서 처리
 */
UCLASS(BlueprintType)
class THIRDMOTION_API USceneController : public UObject
{
	GENERATED_BODY()

public:
	// 초기화
	void Initialize(UWorld* InWorld);

	UFUNCTION(BlueprintCallable, Category = "Scene Controller")
	void SelectActor(AActor* Actor);

	UFUNCTION(BlueprintCallable, Category = "Scene Controller")
	void SelectActors(const TArray<AActor*>& Actors);

	UFUNCTION(BlueprintCallable, Category = "Scene Controller")
	void ClearSelection();

	UFUNCTION(BlueprintCallable, Category = "Scene Controller")
	TArray<AActor*> GetSelectedActors() const;

	// Outline 기능: 가시성 제어
	UFUNCTION(BlueprintCallable, Category = "Scene Controller")
	void ToggleActorVisibility(AActor* Actor);

	UFUNCTION(BlueprintCallable, Category = "Scene Controller")
	void SetActorVisibility(AActor* Actor, bool bVisible);

	// Outline 기능: 이름 변경
	UFUNCTION(BlueprintCallable, Category = "Scene Controller")
	void RenameActor(AActor* Actor, const FString& NewName);

	// Outline 기능: 액터 삭제
	UFUNCTION(BlueprintCallable, Category = "Scene Controller")
	void DeleteActor(AActor* Actor);

	UFUNCTION(BlueprintCallable, Category = "Scene Controller")
	void DeleteSelectedActors();

	// Outline 기능: 액터 복제
	UFUNCTION(BlueprintCallable, Category = "Scene Controller")
	AActor* DuplicateActor(AActor* Actor);

	// 델리게이트: 선택 변경 알림
	DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnSelectionChanged, const TArray<AActor*>&, SelectedActors);
	UPROPERTY(BlueprintAssignable, Category = "Scene Controller")
	FOnSelectionChanged OnSelectionChanged;

	// 델리게이트: 씬 변경 알림 (추가/삭제/수정)
	DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnSceneChanged);
	UPROPERTY(BlueprintAssignable, Category = "Scene Controller")
	FOnSceneChanged OnSceneChanged;

private:
	UPROPERTY()
	UWorld* World;

	UPROPERTY()
	TArray<AActor*> SelectedActors;

	// 변경 알림
	void NotifySelectionChanged();
	void NotifySceneChanged();
};
