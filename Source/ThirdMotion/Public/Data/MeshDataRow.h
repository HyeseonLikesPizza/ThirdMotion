#pragma once

USTRUCT(BlueprintType)
struct FMeshDataRow : public FTableRowBase
{
	GENERATED_BODY()

public:

	UPROPERTY(EditAnywhere)
	FName MeshName;

	UPROPERTY(EditAnywhere)
	
};
