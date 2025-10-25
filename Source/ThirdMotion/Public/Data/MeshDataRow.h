#pragma once

#include "CoreMinimal.h"
#include "MeshDataRow.generated.h"

USTRUCT(BlueprintType)
struct FMeshDataRow : public FTableRowBase
{
	GENERATED_BODY()

public:

	UPROPERTY(EditAnywhere)
	FName MeshName;

	UPROPERTY(EditAnywhere)
	TSoftObjectPtr<UStaticMesh> MeshAssetRef;

	UPROPERTY(EditAnywhere)
	UTexture2D* PreviewImage = nullptr;
	
};
