// Fill out your copyright notice in the Description page of Project Settings.


#include "Edit/PreviewImageGenerator.h"


// Sets default values
APreviewImageGenerator::APreviewImageGenerator()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;


	// 🔹 Root Scene Component 생성
	Root = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
	SetRootComponent(Root);
	
	PreviewSphere = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("SphereMesh"));
	PreviewSphere->SetupAttachment(RootComponent); // ✅ Root 밑에 부착


	// 🔹 기본 Sphere Mesh 로드
	static ConstructorHelpers::FObjectFinder<UStaticMesh> SphereAsset(TEXT("/Engine/BasicShapes/Sphere.Sphere"));
	if (SphereAsset.Succeeded())
	{
		PreviewSphere->SetStaticMesh(SphereAsset.Object);
	}

	// 🔹 Material 로드 (예: /Game/Materials/M_Standard)
	static ConstructorHelpers::FObjectFinder<UMaterial> MatAsset(TEXT("/Game/Blueprints/UI/DefaultMaterials/M_Standard.M_Standard"));
	if (MatAsset.Succeeded())
	{
		PreviewSphere->SetMaterial(0, MatAsset.Object);
	}
}

// Called when the game starts or when spawned
void APreviewImageGenerator::BeginPlay()
{
	Super::BeginPlay();
} 

// Called every frame
void APreviewImageGenerator::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void APreviewImageGenerator::ApplyMaterialToSphere(EMaterialType eMatType)
{
	// switch ()
	// {
	// case EMaterialType::Standard:
	// 	break;
	// }
	//
	
}

