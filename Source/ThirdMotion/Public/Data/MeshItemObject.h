
#pragma once

#include "CoreMinimal.h"
#include "MeshDataRow.h"
#include "UObject/Object.h"
#include "MeshItemObject.generated.h"


UCLASS()
class THIRDMOTION_API UMeshItemObject : public UObject
{
	GENERATED_BODY()

public:
	UPROPERTY(BlueprintReadOnly)
	FMeshDataRow MeshData;
};
