// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "PreviewMaterialRenderer.generated.h"

/**
 * 
 */
UCLASS(Blueprintable) // ✅ 필수

class THIRDMOTION_API UPreviewMaterialRenderer : public UObject
{
	GENERATED_BODY()

public:
	void Initialize(UStaticMesh* InMesh, UMaterialInterface* InMaterial);
	void UpdateMaterialParameter(FName ParamName, float Value);
	UTextureRenderTarget2D* GetRenderTarget() const { return RenderTarget; }

private:
	TSharedPtr<FPreviewScene> PreviewScene;
	
	UPROPERTY()
	UStaticMeshComponent* PreviewMesh = nullptr;
	
	UPROPERTY()
	USceneCaptureComponent2D* Capture = nullptr;
	
	UPROPERTY()
	UMaterialInstanceDynamic* MID = nullptr;
	
	UPROPERTY()
	UTextureRenderTarget2D* RenderTarget = nullptr;
};
