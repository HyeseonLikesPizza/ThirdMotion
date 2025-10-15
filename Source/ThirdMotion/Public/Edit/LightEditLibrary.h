
#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "GameplayTagContainer.h"
#include "Edit/EditTypes.h"
#include "LightEditLibrary.generated.h"

class ULightComponent;
class ALight;
class UEditSyncComponent;


UENUM(BlueprintType)
enum class ELightPropertyType : uint8
{
	Intensity UMETA(DisplayName = "Intensity"),
	LightColor UMETA(DisplayName = "Light Color"),
	AttenuationRadius UMETA(DisplayName = "Attenuation Radius"),
	SourceRadius UMETA(DisplayName = "Source Radius"),
	SourceLength UMETA(DisplayName = "Source Length"),
	InnerConeAngle UMETA(DisplayName = "Inner Cone Angle (Spot)"),
	OuterConeAngle UMETA(DisplayName = "Outer Cone Angle (Spot)"),
	Temperature UMETA(DisplayName = "Temperature"),
	CastShadows UMETA(DisplayName = "Cast Shadows"),
	IndirectLightingIntensity UMETA(DisplayName = "Indirect Lighting Intensity"),
	VolumetricScatteringIntensity UMETA(DisplayName = "Volumetric Scattering")
};

/**
 * Light type classification
 */
UENUM(BlueprintType)
enum class ELightActorType : uint8
{
	Directional UMETA(DisplayName = "Directional Light"),
	Point UMETA(DisplayName = "Point Light"),
	Spot UMETA(DisplayName = "Spot Light"),
	Rect UMETA(DisplayName = "Rect Light"),
	Sky UMETA(DisplayName = "Sky Light")
};

/**
 * Complete light state for save/load
 */
USTRUCT(BlueprintType)
struct FLightState
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float Intensity = 3000.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FLinearColor LightColor = FLinearColor::White;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float AttenuationRadius = 1000.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float SourceRadius = 0.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float SourceLength = 0.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float InnerConeAngle = 0.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float OuterConeAngle = 44.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float Temperature = 6500.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bCastShadows = true;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float IndirectLightingIntensity = 1.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float VolumetricScatteringIntensity = 1.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bUseTemperature = false;
};

/**
 * Blueprint Function Library for Light Actor Editing
 * Provides utilities for manipulating lights through the Edit system
 */
UCLASS()
class THIRDMOTION_API ULightEditLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:
	// ==================== Light Property Getters ====================

	/**
	 * Get light component from actor
	 */
	UFUNCTION(BlueprintCallable, Category = "Light Edit")
	static ULightComponent* GetLightComponent(AActor* LightActor);

	/**
	 * Get light type from actor
	 */
	UFUNCTION(BlueprintCallable, Category = "Light Edit")
	static ELightActorType GetLightType(AActor* LightActor);

	/**
	 * Get complete light state from actor
	 */
	UFUNCTION(BlueprintCallable, Category = "Light Edit")
	static FLightState GetLightState(AActor* LightActor);

	/**
	 * Get specific light property value
	 */
	UFUNCTION(BlueprintCallable, Category = "Light Edit")
	static float GetLightPropertyFloat(AActor* LightActor, ELightPropertyType PropertyType);

	/**
	 * Get light color
	 */
	UFUNCTION(BlueprintCallable, Category = "Light Edit")
	static FLinearColor GetLightColor(AActor* LightActor);

	// ==================== Light Property Setters ====================

	/**
	 * Set complete light state to actor
	 */
	UFUNCTION(BlueprintCallable, Category = "Light Edit")
	static bool SetLightState(AActor* LightActor, const FLightState& State);

	/**
	 * Set light intensity
	 */
	UFUNCTION(BlueprintCallable, Category = "Light Edit")
	static bool SetLightIntensity(AActor* LightActor, float Intensity);

	/**
	 * Set light color
	 */
	UFUNCTION(BlueprintCallable, Category = "Light Edit")
	static bool SetLightColor(AActor* LightActor, FLinearColor Color);

	/**
	 * Set attenuation radius (Point/Spot lights)
	 */
	UFUNCTION(BlueprintCallable, Category = "Light Edit")
	static bool SetAttenuationRadius(AActor* LightActor, float Radius);

	/**
	 * Set source radius (soft shadow)
	 */
	UFUNCTION(BlueprintCallable, Category = "Light Edit")
	static bool SetSourceRadius(AActor* LightActor, float Radius);

	/**
	 * Set cone angles (Spot light only)
	 */
	UFUNCTION(BlueprintCallable, Category = "Light Edit")
	static bool SetSpotLightAngles(AActor* LightActor, float InnerAngle, float OuterAngle);

	/**
	 * Set light temperature
	 */
	UFUNCTION(BlueprintCallable, Category = "Light Edit")
	static bool SetLightTemperature(AActor* LightActor, float Temperature, bool bUseTemperature);

	/**
	 * Set cast shadows
	 */
	UFUNCTION(BlueprintCallable, Category = "Light Edit")
	static bool SetCastShadows(AActor* LightActor, bool bCastShadows);

	// ==================== Property Delta Creation ====================

	/**
	 * Create property delta for light intensity
	 */
	UFUNCTION(BlueprintCallable, Category = "Light Edit")
	static FPropertyDelta MakeLightIntensityDelta(float Intensity);

	/**
	 * Create property delta for light color
	 */
	UFUNCTION(BlueprintCallable, Category = "Light Edit")
	static FPropertyDelta MakeLightColorDelta(FLinearColor Color);

	/**
	 * Create property delta for attenuation radius
	 */
	UFUNCTION(BlueprintCallable, Category = "Light Edit")
	static FPropertyDelta MakeAttenuationRadiusDelta(float Radius);

	/**
	 * Create property delta for source radius
	 */
	UFUNCTION(BlueprintCallable, Category = "Light Edit")
	static FPropertyDelta MakeSourceRadiusDelta(float Radius);

	/**
	 * Create property delta for spot light cone angles
	 */
	UFUNCTION(BlueprintCallable, Category = "Light Edit")
	static FPropertyDelta MakeSpotInnerAngleDelta(float InnerAngle);

	UFUNCTION(BlueprintCallable, Category = "Light Edit")
	static FPropertyDelta MakeSpotOuterAngleDelta(float OuterAngle);

	/**
	 * Create property delta for temperature
	 */
	UFUNCTION(BlueprintCallable, Category = "Light Edit")
	static FPropertyDelta MakeLightTemperatureDelta(float Temperature);

	/**
	 * Create property delta for cast shadows
	 */
	UFUNCTION(BlueprintCallable, Category = "Light Edit")
	static FPropertyDelta MakeCastShadowsDelta(bool bCastShadows);

	// ==================== Edit Sync Integration ====================

	/**
	 * Apply property delta to light through EditSyncComponent
	 */
	UFUNCTION(BlueprintCallable, Category = "Light Edit")
	static bool ApplyLightPropertyDelta(AActor* LightActor, const FPropertyDelta& Delta);

	/**
	 * Get all applicable properties for a light type
	 */
	UFUNCTION(BlueprintCallable, Category = "Light Edit")
	static TArray<ELightPropertyType> GetApplicableProperties(ELightActorType LightType);

	// ==================== Light Presets ====================

	/**
	 * Apply predefined light preset
	 */
	UFUNCTION(BlueprintCallable, Category = "Light Edit")
	static bool ApplyLightPreset(AActor* LightActor, FName PresetName);

	/**
	 * Get default state for light type
	 */
	UFUNCTION(BlueprintCallable, Category = "Light Edit")
	static FLightState GetDefaultLightState(ELightActorType LightType);

	// ==================== Utility ====================

	/**
	 * Convert property type to gameplay tag
	 */
	UFUNCTION(BlueprintCallable, Category = "Light Edit")
	static FGameplayTag PropertyTypeToTag(ELightPropertyType PropertyType);

	/**
	 * Validate light property value range
	 */
	UFUNCTION(BlueprintCallable, Category = "Light Edit")
	static float ClampLightProperty(ELightPropertyType PropertyType, float Value);

	/**
	 * Check if property is supported for light type
	 */
	UFUNCTION(BlueprintCallable, Category = "Light Edit")
	static bool IsPropertySupportedForLightType(ELightPropertyType PropertyType, ELightActorType LightType);

private:
	// Internal helper to get specific light component types
	static class UPointLightComponent* GetPointLightComponent(AActor* Actor);
	static class USpotLightComponent* GetSpotLightComponent(AActor* Actor);
	static class UDirectionalLightComponent* GetDirectionalLightComponent(AActor* Actor);
	static class URectLightComponent* GetRectLightComponent(AActor* Actor);
};
