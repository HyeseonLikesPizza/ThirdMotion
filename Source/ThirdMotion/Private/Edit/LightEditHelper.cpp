// LightEditHelper.cpp
// Copyright Epic Games, Inc. All Rights Reserved.

#include "Edit/LightEditHelper.h"
#include "Edit/EditSyncComponent.h"
#include "Components/LightComponent.h"
#include "Components/PointLightComponent.h"
#include "Components/SpotLightComponent.h"

bool ULightEditHelper::ApplyLightPropertyDelta(AActor* LightActor, const FPropertyDelta& Delta)
{
	if (!LightActor) return false;

	ULightComponent* LC = LightActor->FindComponentByClass<ULightComponent>();
	if (!LC) return false;

	const FName Key = Delta.PropertyTag.GetTagName();

	// Basic Light Properties
	if (Key == "Property.Light.Intensity")
	{
		LC->SetIntensity(Delta.FloatParam);
		LC->MarkRenderStateDirty();
		return true;
	}
	else if (Key == "Property.Light.Color")
	{
		LC->SetLightColor(Delta.ColorParam);
		LC->MarkRenderStateDirty();
		return true;
	}
	else if (Key == "Property.Light.Temperature")
	{
		LC->SetTemperature(Delta.FloatParam);
		LC->MarkRenderStateDirty();
		return true;
	}
	else if (Key == "Property.Light.CastShadows")
	{
		LC->SetCastShadows(Delta.IntParam != 0);
		LC->MarkRenderStateDirty();
		return true;
	}
	else if (Key == "Property.Light.IndirectLighting")
	{
		LC->SetIndirectLightingIntensity(Delta.FloatParam);
		LC->MarkRenderStateDirty();
		return true;
	}
	else if (Key == "Property.Light.VolumetricScattering")
	{
		LC->SetVolumetricScatteringIntensity(Delta.FloatParam);
		LC->MarkRenderStateDirty();
		return true;
	}

	// Point/Spot Light Properties
	if (UPointLightComponent* PLC = Cast<UPointLightComponent>(LC))
	{
		if (Key == "Property.Light.AttenuationRadius")
		{
			PLC->SetAttenuationRadius(Delta.FloatParam);
			PLC->MarkRenderStateDirty();
			return true;
		}
		else if (Key == "Property.Light.SourceRadius")
		{
			PLC->SetSourceRadius(Delta.FloatParam);
			PLC->MarkRenderStateDirty();
			return true;
		}
		else if (Key == "Property.Light.SourceLength")
		{
			PLC->SetSourceLength(Delta.FloatParam);
			PLC->MarkRenderStateDirty();
			return true;
		}
	}

	// Spot Light Properties
	if (USpotLightComponent* SLC = Cast<USpotLightComponent>(LC))
	{
		if (Key == "Property.Light.InnerConeAngle")
		{
			SLC->SetInnerConeAngle(Delta.FloatParam);
			SLC->MarkRenderStateDirty();
			return true;
		}
		else if (Key == "Property.Light.OuterConeAngle")
		{
			SLC->SetOuterConeAngle(Delta.FloatParam);
			SLC->MarkRenderStateDirty();
			return true;
		}
	}

	return false;
}

bool ULightEditHelper::IsLightProperty(const FPropertyDelta& Delta)
{
	const FString TagString = Delta.PropertyTag.ToString();
	return TagString.StartsWith(TEXT("Property.Light."));
}

void ULightEditHelper::RegisterLightPropertyHandler(UEditSyncComponent* EditSync)
{
	// This is a placeholder for future extension
	// You can implement a delegate system in EditSyncComponent if needed
	// For now, users should call ApplyLightPropertyDelta directly
}
