// LightEditLibrary.cpp
// Copyright Epic Games, Inc. All Rights Reserved.

#include "Edit/LightEditLibrary.h"
#include "Engine/Light.h"
#include "Components/LightComponent.h"
#include "Components/PointLightComponent.h"
#include "Components/SpotLightComponent.h"
#include "Components/DirectionalLightComponent.h"
#include "Components/RectLightComponent.h"
#include "Components/SkyLightComponent.h"
#include "Edit/EditSyncComponent.h"
#include "ThirdMotion/ThirdMotion.h"

// ==================== Light Property Getters ====================

ULightComponent* ULightEditLibrary::GetLightComponent(AActor* LightActor)
{
	if (!LightActor) return nullptr;

	// Try to find light component
	return LightActor->FindComponentByClass<ULightComponent>();
}

ELightActorType ULightEditLibrary::GetLightType(AActor* LightActor)
{
	if (!LightActor) return ELightActorType::Point;

	if (GetDirectionalLightComponent(LightActor))
		return ELightActorType::Directional;
	if (GetSpotLightComponent(LightActor))
		return ELightActorType::Spot;
	if (GetRectLightComponent(LightActor))
		return ELightActorType::Rect;
	if (LightActor->FindComponentByClass<USkyLightComponent>())
		return ELightActorType::Sky;

	return ELightActorType::Point;
}

FLightState ULightEditLibrary::GetLightState(AActor* LightActor)
{
	FLightState State;

	ULightComponent* LC = GetLightComponent(LightActor);
	if (!LC) return State;

	State.Intensity = LC->Intensity;
	State.LightColor = LC->LightColor;
	State.bCastShadows = LC->CastShadows;
	State.IndirectLightingIntensity = LC->IndirectLightingIntensity;
	State.VolumetricScatteringIntensity = LC->VolumetricScatteringIntensity;
	State.Temperature = LC->Temperature;
	State.bUseTemperature = LC->bUseTemperature;

	// Point/Spot specific
	if (UPointLightComponent* PLC = GetPointLightComponent(LightActor))
	{
		State.AttenuationRadius = PLC->AttenuationRadius;
		State.SourceRadius = PLC->SourceRadius;
		State.SourceLength = PLC->SourceLength;
	}

	// Spot specific
	if (USpotLightComponent* SLC = GetSpotLightComponent(LightActor))
	{
		State.InnerConeAngle = SLC->InnerConeAngle;
		State.OuterConeAngle = SLC->OuterConeAngle;
	}

	return State;
}

float ULightEditLibrary::GetLightPropertyFloat(AActor* LightActor, ELightPropertyType PropertyType)
{
	ULightComponent* LC = GetLightComponent(LightActor);
	if (!LC) return 0.0f;

	switch (PropertyType)
	{
	case ELightPropertyType::Intensity:
		return LC->Intensity;
	case ELightPropertyType::Temperature:
		return LC->Temperature;
	case ELightPropertyType::IndirectLightingIntensity:
		return LC->IndirectLightingIntensity;
	case ELightPropertyType::VolumetricScatteringIntensity:
		return LC->VolumetricScatteringIntensity;
	case ELightPropertyType::AttenuationRadius:
		if (UPointLightComponent* PLC = GetPointLightComponent(LightActor))
			return PLC->AttenuationRadius;
		break;
	case ELightPropertyType::SourceRadius:
		if (UPointLightComponent* PLC = GetPointLightComponent(LightActor))
			return PLC->SourceRadius;
		break;
	case ELightPropertyType::SourceLength:
		if (UPointLightComponent* PLC = GetPointLightComponent(LightActor))
			return PLC->SourceLength;
		break;
	case ELightPropertyType::InnerConeAngle:
		if (USpotLightComponent* SLC = GetSpotLightComponent(LightActor))
			return SLC->InnerConeAngle;
		break;
	case ELightPropertyType::OuterConeAngle:
		if (USpotLightComponent* SLC = GetSpotLightComponent(LightActor))
			return SLC->OuterConeAngle;
		break;
	}

	return 0.0f;
}

FLinearColor ULightEditLibrary::GetLightColor(AActor* LightActor)
{
	ULightComponent* LC = GetLightComponent(LightActor);
	return LC ? LC->LightColor : FLinearColor::White;
}

// ==================== Light Property Setters ====================

bool ULightEditLibrary::SetLightState(AActor* LightActor, const FLightState& State)
{
	ULightComponent* LC = GetLightComponent(LightActor);
	if (!LC) return false;

	LC->SetIntensity(State.Intensity);
	LC->SetLightColor(State.LightColor);
	LC->SetCastShadows(State.bCastShadows);
	LC->SetIndirectLightingIntensity(State.IndirectLightingIntensity);
	LC->SetVolumetricScatteringIntensity(State.VolumetricScatteringIntensity);
	LC->SetTemperature(State.Temperature);
	LC->SetUseTemperature(State.bUseTemperature);

	if (UPointLightComponent* PLC = GetPointLightComponent(LightActor))
	{
		PLC->SetAttenuationRadius(State.AttenuationRadius);
		PLC->SetSourceRadius(State.SourceRadius);
		PLC->SetSourceLength(State.SourceLength);
	}

	if (USpotLightComponent* SLC = GetSpotLightComponent(LightActor))
	{
		SLC->SetInnerConeAngle(State.InnerConeAngle);
		SLC->SetOuterConeAngle(State.OuterConeAngle);
	}

	LC->MarkRenderStateDirty();
	return true;
}

bool ULightEditLibrary::SetLightIntensity(AActor* LightActor, float Intensity)
{
	ULightComponent* LC = GetLightComponent(LightActor);
	if (!LC) return false;

	LC->SetIntensity(Intensity);
	LC->MarkRenderStateDirty();
	return true;
}

bool ULightEditLibrary::SetLightColor(AActor* LightActor, FLinearColor Color)
{
	ULightComponent* LC = GetLightComponent(LightActor);
	if (!LC) return false;

	LC->SetLightColor(Color);
	LC->MarkRenderStateDirty();
	return true;
}

bool ULightEditLibrary::SetAttenuationRadius(AActor* LightActor, float Radius)
{
	UPointLightComponent* PLC = GetPointLightComponent(LightActor);
	if (!PLC) return false;

	PLC->SetAttenuationRadius(Radius);
	PLC->MarkRenderStateDirty();
	return true;
}

bool ULightEditLibrary::SetSourceRadius(AActor* LightActor, float Radius)
{
	UPointLightComponent* PLC = GetPointLightComponent(LightActor);
	if (!PLC) return false;

	PLC->SetSourceRadius(Radius);
	PLC->MarkRenderStateDirty();
	return true;
}

bool ULightEditLibrary::SetSpotLightAngles(AActor* LightActor, float InnerAngle, float OuterAngle)
{
	USpotLightComponent* SLC = GetSpotLightComponent(LightActor);
	if (!SLC) return false;

	SLC->SetInnerConeAngle(InnerAngle);
	SLC->SetOuterConeAngle(OuterAngle);
	SLC->MarkRenderStateDirty();
	return true;
}

bool ULightEditLibrary::SetLightTemperature(AActor* LightActor, float Temperature, bool bUseTemperature)
{
	ULightComponent* LC = GetLightComponent(LightActor);
	if (!LC) return false;

	LC->SetTemperature(Temperature);
	LC->SetUseTemperature(bUseTemperature);
	LC->MarkRenderStateDirty();
	return true;
}

bool ULightEditLibrary::SetCastShadows(AActor* LightActor, bool bCastShadows)
{
	ULightComponent* LC = GetLightComponent(LightActor);
	if (!LC) return false;

	LC->SetCastShadows(bCastShadows);
	LC->MarkRenderStateDirty();
	return true;
}

// ==================== Property Delta Creation ====================

FPropertyDelta ULightEditLibrary::MakeLightIntensityDelta(float Intensity)
{
	FPropertyDelta Delta;
	Delta.PropertyTag = FGameplayTag::RequestGameplayTag(FName("Property.Light.Intensity"));
	Delta.Op = EPropOp::SetFloat;
	Delta.FloatParam = Intensity;
	return Delta;
}

FPropertyDelta ULightEditLibrary::MakeLightColorDelta(FLinearColor Color)
{
	FPropertyDelta Delta;
	Delta.PropertyTag = FGameplayTag::RequestGameplayTag(FName("Property.Light.Color"));
	Delta.Op = EPropOp::SetColor;
	Delta.ColorParam = Color;
	return Delta;
}

FPropertyDelta ULightEditLibrary::MakeAttenuationRadiusDelta(float Radius)
{
	FPropertyDelta Delta;
	Delta.PropertyTag = FGameplayTag::RequestGameplayTag(FName("Property.Light.AttenuationRadius"));
	Delta.Op = EPropOp::SetFloat;
	Delta.FloatParam = Radius;
	return Delta;
}

FPropertyDelta ULightEditLibrary::MakeSourceRadiusDelta(float Radius)
{
	FPropertyDelta Delta;
	Delta.PropertyTag = FGameplayTag::RequestGameplayTag(FName("Property.Light.SourceRadius"));
	Delta.Op = EPropOp::SetFloat;
	Delta.FloatParam = Radius;
	return Delta;
}

FPropertyDelta ULightEditLibrary::MakeSpotInnerAngleDelta(float InnerAngle)
{
	FPropertyDelta Delta;
	Delta.PropertyTag = FGameplayTag::RequestGameplayTag(FName("Property.Light.InnerConeAngle"));
	Delta.Op = EPropOp::SetFloat;
	Delta.FloatParam = InnerAngle;
	return Delta;
}

FPropertyDelta ULightEditLibrary::MakeSpotOuterAngleDelta(float OuterAngle)
{
	FPropertyDelta Delta;
	Delta.PropertyTag = FGameplayTag::RequestGameplayTag(FName("Property.Light.OuterConeAngle"));
	Delta.Op = EPropOp::SetFloat;
	Delta.FloatParam = OuterAngle;
	return Delta;
}

FPropertyDelta ULightEditLibrary::MakeLightTemperatureDelta(float Temperature)
{
	FPropertyDelta Delta;
	Delta.PropertyTag = FGameplayTag::RequestGameplayTag(FName("Property.Light.Temperature"));
	Delta.Op = EPropOp::SetFloat;
	Delta.FloatParam = Temperature;
	return Delta;
}

FPropertyDelta ULightEditLibrary::MakeCastShadowsDelta(bool bCastShadows)
{
	FPropertyDelta Delta;
	Delta.PropertyTag = FGameplayTag::RequestGameplayTag(FName("Property.Light.CastShadows"));
	Delta.Op = EPropOp::SetInt;
	Delta.IntParam = bCastShadows ? 1 : 0;
	return Delta;
}

// ==================== Edit Sync Integration ====================

bool ULightEditLibrary::ApplyLightPropertyDelta(AActor* LightActor, const FPropertyDelta& Delta)
{
	if (!LightActor) return false;

	ULightComponent* LC = GetLightComponent(LightActor);
	if (!LC) return false;

	const FName Key = Delta.PropertyTag.GetTagName();

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
	else if (Key == "Property.Light.AttenuationRadius")
	{
		if (UPointLightComponent* PLC = GetPointLightComponent(LightActor))
		{
			PLC->SetAttenuationRadius(Delta.FloatParam);
			PLC->MarkRenderStateDirty();
			return true;
		}
	}
	else if (Key == "Property.Light.SourceRadius")
	{
		if (UPointLightComponent* PLC = GetPointLightComponent(LightActor))
		{
			PLC->SetSourceRadius(Delta.FloatParam);
			PLC->MarkRenderStateDirty();
			return true;
		}
	}
	else if (Key == "Property.Light.InnerConeAngle")
	{
		if (USpotLightComponent* SLC = GetSpotLightComponent(LightActor))
		{
			SLC->SetInnerConeAngle(Delta.FloatParam);
			SLC->MarkRenderStateDirty();
			return true;
		}
	}
	else if (Key == "Property.Light.OuterConeAngle")
	{
		if (USpotLightComponent* SLC = GetSpotLightComponent(LightActor))
		{
			SLC->SetOuterConeAngle(Delta.FloatParam);
			SLC->MarkRenderStateDirty();
			return true;
		}
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

	return false;
}

TArray<ELightPropertyType> ULightEditLibrary::GetApplicableProperties(ELightActorType LightType)
{
	TArray<ELightPropertyType> Properties;

	// Common properties for all lights
	Properties.Add(ELightPropertyType::Intensity);
	Properties.Add(ELightPropertyType::LightColor);
	Properties.Add(ELightPropertyType::Temperature);
	Properties.Add(ELightPropertyType::CastShadows);
	Properties.Add(ELightPropertyType::IndirectLightingIntensity);
	Properties.Add(ELightPropertyType::VolumetricScatteringIntensity);

	// Type-specific properties
	switch (LightType)
	{
	case ELightActorType::Point:
		Properties.Add(ELightPropertyType::AttenuationRadius);
		Properties.Add(ELightPropertyType::SourceRadius);
		Properties.Add(ELightPropertyType::SourceLength);
		break;

	case ELightActorType::Spot:
		Properties.Add(ELightPropertyType::AttenuationRadius);
		Properties.Add(ELightPropertyType::SourceRadius);
		Properties.Add(ELightPropertyType::InnerConeAngle);
		Properties.Add(ELightPropertyType::OuterConeAngle);
		break;

	case ELightActorType::Rect:
		Properties.Add(ELightPropertyType::AttenuationRadius);
		Properties.Add(ELightPropertyType::SourceRadius);
		break;

	case ELightActorType::Directional:
	case ELightActorType::Sky:
		// Only common properties
		break;
	}

	return Properties;
}

// ==================== Light Presets ====================

bool ULightEditLibrary::ApplyLightPreset(AActor* LightActor, FName PresetName)
{
	ELightActorType LightType = GetLightType(LightActor);
	FLightState State;

	if (PresetName == "Bright")
	{
		State = GetDefaultLightState(LightType);
		State.Intensity *= 2.0f;
	}
	else if (PresetName == "Dim")
	{
		State = GetDefaultLightState(LightType);
		State.Intensity *= 0.5f;
	}
	else if (PresetName == "Warm")
	{
		State = GetDefaultLightState(LightType);
		State.bUseTemperature = true;
		State.Temperature = 3000.0f;
	}
	else if (PresetName == "Cool")
	{
		State = GetDefaultLightState(LightType);
		State.bUseTemperature = true;
		State.Temperature = 8000.0f;
	}
	else
	{
		State = GetDefaultLightState(LightType);
	}

	return SetLightState(LightActor, State);
}

FLightState ULightEditLibrary::GetDefaultLightState(ELightActorType LightType)
{
	FLightState State;

	switch (LightType)
	{
	case ELightActorType::Directional:
		State.Intensity = 10.0f;
		State.LightColor = FLinearColor(1.0f, 0.97f, 0.86f); // Slightly warm
		State.bCastShadows = true;
		break;

	case ELightActorType::Point:
		State.Intensity = 5000.0f;
		State.AttenuationRadius = 1000.0f;
		State.SourceRadius = 10.0f;
		State.bCastShadows = true;
		break;

	case ELightActorType::Spot:
		State.Intensity = 5000.0f;
		State.AttenuationRadius = 1000.0f;
		State.InnerConeAngle = 0.0f;
		State.OuterConeAngle = 44.0f;
		State.bCastShadows = true;
		break;

	case ELightActorType::Rect:
		State.Intensity = 5000.0f;
		State.AttenuationRadius = 1000.0f;
		State.SourceRadius = 50.0f;
		State.bCastShadows = true;
		break;

	case ELightActorType::Sky:
		State.Intensity = 1.0f;
		State.LightColor = FLinearColor(0.5f, 0.7f, 1.0f); // Sky blue
		break;
	}

	return State;
}

// ==================== Utility ====================

FGameplayTag ULightEditLibrary::PropertyTypeToTag(ELightPropertyType PropertyType)
{
	switch (PropertyType)
	{
	case ELightPropertyType::Intensity:
		return FGameplayTag::RequestGameplayTag(FName("Property.Light.Intensity"));
	case ELightPropertyType::LightColor:
		return FGameplayTag::RequestGameplayTag(FName("Property.Light.Color"));
	case ELightPropertyType::AttenuationRadius:
		return FGameplayTag::RequestGameplayTag(FName("Property.Light.AttenuationRadius"));
	case ELightPropertyType::SourceRadius:
		return FGameplayTag::RequestGameplayTag(FName("Property.Light.SourceRadius"));
	case ELightPropertyType::SourceLength:
		return FGameplayTag::RequestGameplayTag(FName("Property.Light.SourceLength"));
	case ELightPropertyType::InnerConeAngle:
		return FGameplayTag::RequestGameplayTag(FName("Property.Light.InnerConeAngle"));
	case ELightPropertyType::OuterConeAngle:
		return FGameplayTag::RequestGameplayTag(FName("Property.Light.OuterConeAngle"));
	case ELightPropertyType::Temperature:
		return FGameplayTag::RequestGameplayTag(FName("Property.Light.Temperature"));
	case ELightPropertyType::CastShadows:
		return FGameplayTag::RequestGameplayTag(FName("Property.Light.CastShadows"));
	case ELightPropertyType::IndirectLightingIntensity:
		return FGameplayTag::RequestGameplayTag(FName("Property.Light.IndirectLighting"));
	case ELightPropertyType::VolumetricScatteringIntensity:
		return FGameplayTag::RequestGameplayTag(FName("Property.Light.VolumetricScattering"));
	}

	return FGameplayTag();
}

float ULightEditLibrary::ClampLightProperty(ELightPropertyType PropertyType, float Value)
{
	switch (PropertyType)
	{
	case ELightPropertyType::Intensity:
		return FMath::Max(0.0f, Value);
	case ELightPropertyType::AttenuationRadius:
		return FMath::Clamp(Value, 0.0f, 100000.0f);
	case ELightPropertyType::SourceRadius:
		return FMath::Clamp(Value, 0.0f, 1000.0f);
	case ELightPropertyType::InnerConeAngle:
		return FMath::Clamp(Value, 0.0f, 89.0f);
	case ELightPropertyType::OuterConeAngle:
		return FMath::Clamp(Value, 1.0f, 89.0f);
	case ELightPropertyType::Temperature:
		return FMath::Clamp(Value, 1700.0f, 12000.0f);
	case ELightPropertyType::IndirectLightingIntensity:
	case ELightPropertyType::VolumetricScatteringIntensity:
		return FMath::Clamp(Value, 0.0f, 10.0f);
	}

	return Value;
}

bool ULightEditLibrary::IsPropertySupportedForLightType(ELightPropertyType PropertyType, ELightActorType LightType)
{
	TArray<ELightPropertyType> Applicable = GetApplicableProperties(LightType);
	return Applicable.Contains(PropertyType);
}

// ==================== Private Helpers ====================

UPointLightComponent* ULightEditLibrary::GetPointLightComponent(AActor* Actor)
{
	return Actor ? Actor->FindComponentByClass<UPointLightComponent>() : nullptr;
}

USpotLightComponent* ULightEditLibrary::GetSpotLightComponent(AActor* Actor)
{
	return Actor ? Actor->FindComponentByClass<USpotLightComponent>() : nullptr;
}

UDirectionalLightComponent* ULightEditLibrary::GetDirectionalLightComponent(AActor* Actor)
{
	return Actor ? Actor->FindComponentByClass<UDirectionalLightComponent>() : nullptr;
}

URectLightComponent* ULightEditLibrary::GetRectLightComponent(AActor* Actor)
{
	return Actor ? Actor->FindComponentByClass<URectLightComponent>() : nullptr;
}
