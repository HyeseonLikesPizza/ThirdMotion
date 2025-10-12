// LightEditHelper.h - Helper class for Light Actor editing (extends EditSyncComponent)
// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "Edit/EditTypes.h"
#include "LightEditHelper.generated.h"

class ULightComponent;
class AActor;
class UEditSyncComponent;

/**
 * Helper class to extend EditSyncComponent functionality for Light editing
 * Does not modify original EditSyncComponent code
 */
UCLASS()
class THIRDMOTION_API ULightEditHelper : public UObject
{
	GENERATED_BODY()

public:
	/**
	 * Apply light-specific property delta to actor
	 * Call this from your own code to handle light properties
	 */
	UFUNCTION(BlueprintCallable, Category = "Light Edit Helper")
	static bool ApplyLightPropertyDelta(AActor* LightActor, const FPropertyDelta& Delta);

	/**
	 * Check if property delta is a light property
	 */
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Light Edit Helper")
	static bool IsLightProperty(const FPropertyDelta& Delta);

	/**
	 * Register this helper with EditSyncComponent (optional)
	 * Can be called in your game initialization
	 */
	UFUNCTION(BlueprintCallable, Category = "Light Edit Helper")
	static void RegisterLightPropertyHandler(UEditSyncComponent* EditSync);
};
