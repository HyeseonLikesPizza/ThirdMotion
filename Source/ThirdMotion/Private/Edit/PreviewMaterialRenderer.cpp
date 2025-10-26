// Fill out your copyright notice in the Description page of Project Settings.


#include "Edit/PreviewMaterialRenderer.h"

#include "Components/DirectionalLightComponent.h"
#include "Components/SceneCaptureComponent2D.h"
#include "Components/SpotLightComponent.h"
#include "Engine/TextureRenderTarget2D.h"

void UPreviewMaterialRenderer::Initialize(UStaticMesh* InMesh, UMaterialInterface* InMaterial)
{
	PreviewScene = MakeShareable(new FPreviewScene(FPreviewScene::ConstructionValues()));
	
	// Sphere Mesh 추가
	PreviewMesh = NewObject<UStaticMeshComponent>();
	PreviewMesh->SetStaticMesh(InMesh);
	PreviewMesh->SetMobility(EComponentMobility::Movable);
	PreviewScene->AddComponent(PreviewMesh, FTransform::Identity);
	
	// // 머티리얼 인스턴스 생성
	// MID = UMaterialInstanceDynamic::Create(InMaterial, this);
	// PreviewMesh->SetMaterial(0, MID);
	
	
	// 3️⃣ 머티리얼 로드 및 Dynamic Material 생성
	UMaterialInterface* BaseMaterial = LoadObject<UMaterialInterface>(nullptr, TEXT("/Game/Blueprints/UI/DefaultMaterials/M_Standard.M_Standard"));
 
	// 머티리얼 인스턴스 생성
	if (BaseMaterial)
	{
		MID = UMaterialInstanceDynamic::Create(BaseMaterial, this);
		if (PreviewMesh)
		{
			PreviewMesh->SetMaterial(0, MID);
		}
	}
	
	// 4️⃣ RenderTarget 로드
	RenderTarget = LoadObject<UTextureRenderTarget2D>(nullptr, TEXT("/Game/Blueprints/UI/PrevieImageGenerator/BP_PreviewImage.BP_PreviewImage"));
	// RenderTarget = LoadObject<UTextureRenderTarget2D>(nullptr, TEXT("/Engine/Functions/Engine_MaterialFunctions02/ExampleContent/Textures/testPattern2.testPattern2"));
 
	
	// Capture 컴포넌트 생성
	Capture = NewObject<USceneCaptureComponent2D>();
	PreviewScene->AddComponent(Capture, FTransform::Identity);
	Capture->SetRelativeLocation(FVector(-77.0f, 0.0f, 0.0f));
	
	
	Capture->TextureTarget = RenderTarget;
	Capture->CaptureSource = ESceneCaptureSource::SCS_FinalColorLDR;
	
	// 1️⃣ SpotLightComponent 생성
	USpotLightComponent* SpotLight = NewObject<USpotLightComponent>();
	
	// 2️⃣ FPreviewScene에 추가
	PreviewScene->AddComponent(SpotLight, FTransform(FRotator(-40.0f, 0.0f, 0.0f), FVector(-46.422537f, 0.0f, 24.525821f)));
	
	// 3️⃣ 활성화 및 설정
	SpotLight->SetIntensity(5000.0f);          // 필요에 따라 조정
	SpotLight->SetLightColor(FLinearColor::White);
	SpotLight->SetVisibility(true);
	SpotLight->RegisterComponent();           // 반드시 등록
	SpotLight->SetOuterConeAngle(90.0f);

	// ⚠️ PreviewScene 안에서는 Tick 필요 없음, CaptureScene() 호출 시 렌더링됨
	
	// 초기 캡처
	Capture->CaptureScene();
	
}

void UPreviewMaterialRenderer::UpdateMaterialParameter(FName ParamName, float Value)
{
	if (MID)
	{
		MID->SetScalarParameterValue(ParamName, Value);
		if (Capture)
			Capture->CaptureScene(); // 머티리얼 변경 시 즉시 갱신
	}
}
