# Light Edit System - 사용 가이드

Light Actor 편집을 위한 완전한 라이브러리 시스템입니다.

## 📋 목차

1. [개요](#개요)
2. [시스템 구조](#시스템-구조)
3. [설정 방법](#설정-방법)
4. [사용 예제](#사용-예제)
5. [API 레퍼런스](#api-레퍼런스)

---

## 개요

### 주요 기능

✅ **모든 Light 타입 지원**
- Directional Light (태양광, 달빛)
- Point Light (전구, 촛불)
- Spot Light (스포트라이트, 손전등)
- Rect Light (소프트박스, 패널 조명)
- Sky Light (환경광)

✅ **완전한 속성 제어**
- Intensity (밝기)
- Color (색상)
- Attenuation Radius (감쇠 반경)
- Source Radius (광원 크기)
- Cone Angles (Spot Light 각도)
- Temperature (색온도)
- Cast Shadows (그림자 생성)
- Indirect Lighting (간접 조명)
- Volumetric Scattering (볼류메트릭 효과)

✅ **네트워크 동기화**
- EditSyncComponent 통합
- 서버-클라이언트 자동 복제
- Property Delta 시스템

✅ **프리셋 시스템**
- 사전 정의된 조명 프리셋
- DataTable 기반 관리
- 런타임 스폰 지원

---

## 시스템 구조

### 파일 구조

```
Source/ThirdMotion/
├── Public/Edit/
│   ├── LightEditLibrary.h         # 메인 라이브러리 (Blueprint Function Library)
│   ├── EditTypes.h                # 공통 데이터 타입
│   ├── EditSyncComponent.h        # 동기화 컴포넌트
│   ├── AssetResolver.h            # 프리셋 관리
│   └── SceneManager.h             # Scene 관리
│
├── Private/Edit/
│   ├── LightEditLibrary.cpp       # 라이브러리 구현
│   ├── EditSyncComponent.cpp      # 동기화 로직 (업데이트됨)
│   ├── AssetResolver.cpp
│   └── SceneManager.cpp
│
Content/Data/
└── LightPresetsExample.json       # 프리셋 예제
```

### 클래스 다이어그램

```
UBlueprintFunctionLibrary
    ↓
ULightEditLibrary
    ├── GetLightComponent()
    ├── GetLightState()
    ├── SetLightState()
    ├── MakeLightIntensityDelta()
    ├── ApplyLightPropertyDelta()
    └── ApplyLightPreset()

UActorComponent
    ↓
UEditSyncComponent
    ├── R_Meta (Replicated)
    ├── R_PropsAppliedHistory (Replicated)
    ├── ApplyDeltaToNative()
    └── ServerApplyPropertyDelta_Internal()

UWorldSubsystem
    ↓
USceneManager
    ├── SpawnByTag()
    ├── ApplyPropertyDelta()
    └── FindByGuid()
```

---

## 설정 방법

### 1. 컴파일

```bash
# Visual Studio에서
1. ThirdMotion.sln 열기
2. Build > Build Solution (Ctrl+Shift+B)
3. 성공 확인
```

### 2. GameplayTag 설정

**Project Settings > GameplayTags**에 다음 태그 추가:

```
Preset.Light.Directional.Sun
Preset.Light.Point.Basic
Preset.Light.Spot.Basic
Preset.Light.Rect.Basic

Property.Light.Intensity
Property.Light.Color
Property.Light.AttenuationRadius
Property.Light.SourceRadius
Property.Light.SourceLength
Property.Light.InnerConeAngle
Property.Light.OuterConeAngle
Property.Light.Temperature
Property.Light.CastShadows
Property.Light.IndirectLighting
Property.Light.VolumetricScattering
```

또는 `.ini` 파일에 직접 추가:

**Config/DefaultGameplayTags.ini**:
```ini
[/Script/GameplayTags.GameplayTagsList]
+GameplayTagList=(Tag="Preset.Light",DevComment="Light Presets")
+GameplayTagList=(Tag="Preset.Light.Directional",DevComment="")
+GameplayTagList=(Tag="Preset.Light.Point",DevComment="")
+GameplayTagList=(Tag="Preset.Light.Spot",DevComment="")

+GameplayTagList=(Tag="Property.Light",DevComment="Light Properties")
+GameplayTagList=(Tag="Property.Light.Intensity",DevComment="")
+GameplayTagList=(Tag="Property.Light.Color",DevComment="")
+GameplayTagList=(Tag="Property.Light.AttenuationRadius",DevComment="")
```

### 3. DataTable 생성

1. Content Browser에서 우클릭 > Miscellaneous > Data Table
2. Row Structure: `FLibraryRow` 선택
3. 이름: `DT_LightPresets`
4. `LightPresetsExample.json` 데이터 import

### 4. AssetResolver 초기화

**GameMode BeginPlay**:
```cpp
UAssetResolver* Resolver = GetWorld()->GetSubsystem<UAssetResolver>();
UDataTable* LightPresetsTable = LoadObject<UDataTable>(nullptr, TEXT("/Game/Data/DT_LightPresets"));
Resolver->BuildIndex(LightPresetsTable);
```

---

## 사용 예제

### 예제 1: Light 속성 직접 변경 (로컬)

```cpp
// C++
#include "Edit/LightEditLibrary.h"

void AMyActor::ChangeLightIntensity(AActor* LightActor, float NewIntensity)
{
    if (ULightEditLibrary::SetLightIntensity(LightActor, NewIntensity))
    {
        UE_LOG(LogTemp, Log, TEXT("Light intensity changed to %f"), NewIntensity);
    }
}
```

**Blueprint**:
```
On Button Clicked
└── Light Edit Library > Set Light Intensity
    ├── Light Actor: [Reference]
    └── Intensity: 10000.0
```

### 예제 2: Property Delta를 통한 네트워크 동기화

```cpp
// Server
void AMyPlayerController::Server_ChangeLightColor(AActor* LightActor, FLinearColor NewColor)
{
    UEditSyncComponent* EditSync = LightActor->FindComponentByClass<UEditSyncComponent>();
    if (!EditSync) return;

    FGuid LightGuid = EditSync->GetMeta().Guid;
    FPropertyDelta Delta = ULightEditLibrary::MakeLightColorDelta(NewColor);

    USceneManager* SceneMgr = GetWorld()->GetSubsystem<USceneManager>();
    SceneMgr->ApplyPropertyDelta(LightGuid, Delta);
}
```

**Blueprint**:
```
On Color Picker Changed (Color)
└── Get Edit Sync Component (Light Actor)
    └── Get Meta
        └── Get Guid
            └── Make Light Color Delta (Color)
                └── Get World Subsystem (Scene Manager)
                    └── Apply Property Delta
                        ├── Guid: [From Above]
                        └── Delta: [From Above]
```

### 예제 3: 프리셋으로 Light 스폰

```cpp
// Server
AActor* AMyGameMode::SpawnLightFromPreset(FGameplayTag PresetTag, FVector Location)
{
    USceneManager* SceneMgr = GetWorld()->GetSubsystem<USceneManager>();

    FTransform SpawnTransform;
    SpawnTransform.SetLocation(Location);
    SpawnTransform.SetRotation(FQuat::Identity);

    AActor* NewLight = SceneMgr->SpawnByTag(PresetTag, SpawnTransform);

    if (NewLight)
    {
        UE_LOG(LogTemp, Log, TEXT("Spawned light: %s"), *NewLight->GetName());
    }

    return NewLight;
}
```

**Blueprint**:
```
On Spawn Button Clicked
└── Make Gameplay Tag (Preset.Light.Point.Basic)
    └── Get World Subsystem (Scene Manager)
        └── Spawn By Tag
            ├── Preset Tag: [Tag]
            └── Transform: [Location/Rotation]
```

### 예제 4: Light 상태 저장/복원

```cpp
// Save
FLightState SavedState;

void AMyActor::SaveLightState(AActor* LightActor)
{
    SavedState = ULightEditLibrary::GetLightState(LightActor);
    UE_LOG(LogTemp, Log, TEXT("Saved light state - Intensity: %f"), SavedState.Intensity);
}

// Restore
void AMyActor::RestoreLightState(AActor* LightActor)
{
    ULightEditLibrary::SetLightState(LightActor, SavedState);
    UE_LOG(LogTemp, Log, TEXT("Restored light state"));
}
```

### 예제 5: 프리셋 적용

```cpp
// C++
ULightEditLibrary::ApplyLightPreset(LightActor, FName("Bright"));
// 또는
ULightEditLibrary::ApplyLightPreset(LightActor, FName("Warm"));
```

**Blueprint**:
```
On Preset Dropdown Changed (Preset Name)
└── Light Edit Library > Apply Light Preset
    ├── Light Actor: [Reference]
    └── Preset Name: [Selected Name]
```

### 예제 6: Spot Light 각도 조정

```cpp
void AMyActor::AdjustSpotLightAngles(AActor* SpotLight, float Inner, float Outer)
{
    // Validate
    Inner = FMath::Clamp(Inner, 0.0f, 89.0f);
    Outer = FMath::Clamp(Outer, Inner + 1.0f, 89.0f);

    ULightEditLibrary::SetSpotLightAngles(SpotLight, Inner, Outer);
}
```

### 예제 7: 색온도 제어

```cpp
void AMyActor::SetWarmLight(AActor* LightActor)
{
    ULightEditLibrary::SetLightTemperature(LightActor, 3000.0f, true);
}

void AMyActor::SetCoolLight(AActor* LightActor)
{
    ULightEditLibrary::SetLightTemperature(LightActor, 8000.0f, true);
}

void AMyActor::SetNeutralLight(AActor* LightActor)
{
    ULightEditLibrary::SetLightTemperature(LightActor, 6500.0f, true);
}
```

### 예제 8: UI에서 Light 타입별 속성 표시

```cpp
void UMyLightPropertiesWidget::UpdatePropertiesForLight(AActor* LightActor)
{
    ELightActorType LightType = ULightEditLibrary::GetLightType(LightActor);
    TArray<ELightPropertyType> ApplicableProps =
        ULightEditLibrary::GetApplicableProperties(LightType);

    // UI 업데이트
    for (ELightPropertyType PropType : ApplicableProps)
    {
        float Value = ULightEditLibrary::GetLightPropertyFloat(LightActor, PropType);
        UpdateUISlider(PropType, Value);
    }
}
```

**Blueprint**:
```
On Light Selected (Light Actor)
└── Get Light Type
    └── Get Applicable Properties
        └── For Each Loop
            ├── Get Light Property Float
            │   ├── Light Actor: [Selected]
            │   └── Property Type: [Loop Item]
            └── Update UI Slider
                ├── Property Type: [Loop Item]
                └── Value: [Result]
```

---

## API 레퍼런스

### LightEditLibrary 주요 함수

#### Getters

```cpp
// Light 컴포넌트 가져오기
ULightComponent* GetLightComponent(AActor* LightActor);

// Light 타입 확인
ELightActorType GetLightType(AActor* LightActor);

// 전체 Light 상태 가져오기
FLightState GetLightState(AActor* LightActor);

// 특정 속성 값 가져오기
float GetLightPropertyFloat(AActor* LightActor, ELightPropertyType PropertyType);

// Light 색상 가져오기
FLinearColor GetLightColor(AActor* LightActor);
```

#### Setters

```cpp
// 전체 Light 상태 설정
bool SetLightState(AActor* LightActor, const FLightState& State);

// 밝기 설정
bool SetLightIntensity(AActor* LightActor, float Intensity);

// 색상 설정
bool SetLightColor(AActor* LightActor, FLinearColor Color);

// 감쇠 반경 설정 (Point/Spot)
bool SetAttenuationRadius(AActor* LightActor, float Radius);

// 광원 크기 설정
bool SetSourceRadius(AActor* LightActor, float Radius);

// Spot Light 각도 설정
bool SetSpotLightAngles(AActor* LightActor, float InnerAngle, float OuterAngle);

// 색온도 설정
bool SetLightTemperature(AActor* LightActor, float Temperature, bool bUseTemperature);

// 그림자 설정
bool SetCastShadows(AActor* LightActor, bool bCastShadows);
```

#### Property Delta 생성

```cpp
// Delta 생성 함수들
FPropertyDelta MakeLightIntensityDelta(float Intensity);
FPropertyDelta MakeLightColorDelta(FLinearColor Color);
FPropertyDelta MakeAttenuationRadiusDelta(float Radius);
FPropertyDelta MakeSourceRadiusDelta(float Radius);
FPropertyDelta MakeSpotInnerAngleDelta(float InnerAngle);
FPropertyDelta MakeSpotOuterAngleDelta(float OuterAngle);
FPropertyDelta MakeLightTemperatureDelta(float Temperature);
FPropertyDelta MakeCastShadowsDelta(bool bCastShadows);
```

#### 유틸리티

```cpp
// Property 타입을 Gameplay Tag로 변환
FGameplayTag PropertyTypeToTag(ELightPropertyType PropertyType);

// Property 값 범위 제한
float ClampLightProperty(ELightPropertyType PropertyType, float Value);

// Light 타입에서 지원하는 속성인지 확인
bool IsPropertySupportedForLightType(ELightPropertyType PropertyType, ELightActorType LightType);

// Light 타입별 기본 상태 가져오기
FLightState GetDefaultLightState(ELightActorType LightType);

// 프리셋 적용
bool ApplyLightPreset(AActor* LightActor, FName PresetName);
```

### FLightState 구조체

```cpp
USTRUCT(BlueprintType)
struct FLightState
{
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
```

### 열거형

#### ELightActorType

```cpp
enum class ELightActorType : uint8
{
    Directional,  // 태양광, 달빛
    Point,        // 전구, 촛불
    Spot,         // 스포트라이트
    Rect,         // 소프트박스
    Sky           // 환경광
};
```

#### ELightPropertyType

```cpp
enum class ELightPropertyType : uint8
{
    Intensity,                      // 밝기
    LightColor,                     // 색상
    AttenuationRadius,              // 감쇠 반경
    SourceRadius,                   // 광원 크기
    SourceLength,                   // 광원 길이
    InnerConeAngle,                 // Spot 내부 각도
    OuterConeAngle,                 // Spot 외부 각도
    Temperature,                    // 색온도
    CastShadows,                    // 그림자 생성
    IndirectLightingIntensity,      // 간접 조명
    VolumetricScatteringIntensity   // 볼류메트릭
};
```

---

## 고급 사용법

### 커스텀 프리셋 추가

**C++**:
```cpp
void UMyLibraryManager::AddCustomLightPreset()
{
    FLibraryRow NewPreset;
    NewPreset.DisplayName = FName("Custom Warm Light");
    NewPreset.PresetTag = FGameplayTag::RequestGameplayTag(
        FName("Preset.Light.Custom.Warm")
    );
    NewPreset.ClassRef = TSoftClassPtr<AActor>(
        FSoftObjectPath(TEXT("/Script/Engine.PointLight"))
    );

    // DataTable에 추가
    MyDataTable->AddRow(FName("Custom_Warm"), NewPreset);
}
```

### 애니메이션으로 Light 변경

```cpp
void AMyActor::AnimateLightIntensity(AActor* LightActor, float TargetIntensity, float Duration)
{
    float CurrentIntensity = ULightEditLibrary::GetLightPropertyFloat(
        LightActor, ELightPropertyType::Intensity
    );

    FTimerHandle TimerHandle;
    float ElapsedTime = 0.0f;

    GetWorld()->GetTimerManager().SetTimer(TimerHandle, [=]() mutable
    {
        ElapsedTime += 0.016f; // ~60 FPS
        float Alpha = FMath::Clamp(ElapsedTime / Duration, 0.0f, 1.0f);
        float NewIntensity = FMath::Lerp(CurrentIntensity, TargetIntensity, Alpha);

        ULightEditLibrary::SetLightIntensity(LightActor, NewIntensity);

        if (Alpha >= 1.0f)
        {
            GetWorld()->GetTimerManager().ClearTimer(TimerHandle);
        }
    }, 0.016f, true);
}
```

### 배치 Light 편집

```cpp
void AMyActor::SetAllLightsIntensity(const TArray<AActor*>& Lights, float Intensity)
{
    for (AActor* Light : Lights)
    {
        ULightEditLibrary::SetLightIntensity(Light, Intensity);
    }
}
```

---

## 문제 해결

### Light가 변경되지 않음

**확인 사항**:
1. LightComponent가 존재하는지 확인
2. Actor가 Light 클래스인지 확인
3. MarkRenderStateDirty() 호출 여부

**해결**:
```cpp
ULightComponent* LC = ULightEditLibrary::GetLightComponent(LightActor);
if (LC)
{
    LC->SetIntensity(NewValue);
    LC->MarkRenderStateDirty();
}
```

### 네트워크 동기화 안 됨

**확인 사항**:
1. EditSyncComponent 부착 여부
2. Actor Replicates 설정
3. SceneManager를 통한 변경인지 확인

**해결**:
```cpp
// 서버에서만
if (GetWorld()->GetNetMode() != NM_Client)
{
    USceneManager* SceneMgr = GetWorld()->GetSubsystem<USceneManager>();
    SceneMgr->ApplyPropertyDelta(Guid, Delta);
}
```

### GameplayTag를 찾을 수 없음

**해결**: Project Settings > GameplayTags에서 태그 추가 또는 `.ini` 파일 수정

---

## 성능 최적화

### 배치 처리

```cpp
// Bad - 개별 호출
for (AActor* Light : Lights)
{
    ULightEditLibrary::SetLightIntensity(Light, Value);
}

// Good - 배치 + 단일 MarkRenderStateDirty
TArray<ULightComponent*> Components;
for (AActor* Light : Lights)
{
    ULightComponent* LC = ULightEditLibrary::GetLightComponent(Light);
    if (LC)
    {
        LC->SetIntensity(Value);
        Components.Add(LC);
    }
}

// 마지막에 한 번만
for (ULightComponent* LC : Components)
{
    LC->MarkRenderStateDirty();
}
```

---

**버전**: 1.0
**작성일**: 2025
**Unreal Engine**: 5.3
