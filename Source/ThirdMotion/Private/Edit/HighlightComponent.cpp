
#include "Edit/HighlightComponent.h"


UHighlightComponent::UHighlightComponent()
{
	PrimaryComponentTick.bCanEverTick = false;


}

void UHighlightComponent::EnableHighlight(bool bEnable)
{
	AActor* Owner = GetOwner();
	if (!Owner) return;

	TArray<UPrimitiveComponent*> Prims;
	Owner->GetComponents<UPrimitiveComponent>(Prims, false);

	for (UPrimitiveComponent* Prim : Prims)
	{
		if (!Prim) continue;

		Prim->SetRenderCustomDepth(bEnable);
		if (bEnable)
		{
			Prim->SetCustomDepthStencilValue(StencilValue);
		}
	}
}
