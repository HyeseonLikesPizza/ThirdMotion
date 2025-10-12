# Twinmotion 2023.1 스타일 UI 시스템

Unreal Engine 5.3 UMG (Widget Blueprint)를 사용하여 Twinmotion 2023.1의 전체 인터페이스를 재현한 시스템입니다.

## 📋 목차

1. [개요](#개요)
2. [시스템 구조](#시스템-구조)
3. [설치 및 설정](#설치-및-설정)
4. [사용 방법](#사용-방법)
5. [커스터마이징](#커스터마이징)
6. [문제 해결](#문제-해결)

## 개요

### 주요 기능

- ✅ Twinmotion 스타일의 완전한 UI 레이아웃
- ✅ 좌측 Library 패널 (에셋 브라우저)
- ✅ 중앙 3D 뷰포트
- ✅ 우측 Scene/Properties 패널
- ✅ 하단 썸네일 뷰 및 탭 시스템
- ✅ 상단 메뉴 및 툴바
- ✅ C++ 기반 UI 상태 관리 시스템
- ✅ Blueprint 확장 가능한 구조

### 지원 기능

- 패널 표시/숨김 토글
- Scene 계층 구조 관리
- 썸네일 캡처 및 관리
- 오브젝트 선택 및 Properties 업데이트
- 라이브러리 에셋 검색 및 필터링

## 시스템 구조

### 파일 구조

```
ThirdMotion/
├── Source/ThirdMotion/
│   ├── TwinmotionUIManager.h/cpp          # UI 상태 관리 시스템
│   └── UI/
│       ├── TwinmotionBaseWidget.h/cpp     # 모든 위젯의 베이스 클래스
│       └── TwinmotionMainWidget.h/cpp     # 메인 컨테이너 위젯
│
└── Content/BluePrint/UI/
    ├── WBP_TwinmotionUI.uasset            # 메인 UI (생성 필요)
    ├── Widgets/
    │   ├── WBP_ThumbnailItem.uasset       # 썸네일 아이템 (생성 필요)
    │   └── Panels/
    │       ├── WBP_TopBar.uasset          # 상단 툴바 (생성 필요)
    │       ├── WBP_LeftPanel.uasset       # 라이브러리 패널 (생성 필요)
    │       ├── WBP_RightPanel.uasset      # Scene/Properties (생성 필요)
    │       └── WBP_BottomPanel.uasset     # 썸네일/탭 패널 (생성 필요)
    │
    ├── CreateTwinmotionUI.py              # Python 생성 스크립트
    ├── TWINMOTION_UI_STRUCTURE.md         # 상세 구조 문서
    ├── QUICK_START_GUIDE.md               # 빠른 시작 가이드
    └── README.md                           # 이 파일
```

### 아키텍처 다이어그램

```
┌─────────────────────────────────────────────────┐
│          WBP_TwinmotionUI (Main)                │
│  ┌───────────────────────────────────────────┐  │
│  │         WBP_TopBar (48px)                 │  │
│  └───────────────────────────────────────────┘  │
│  ┌───────┬───────────────────┬──────────────┐  │
│  │ WBP_  │   Viewport        │   WBP_Right  │  │
│  │ Left  │   Container       │   Panel      │  │
│  │ Panel │   (3D View)       │   (Scene/    │  │
│  │ 280px │                   │   Props)     │  │
│  │       │                   │   320px      │  │
│  └───────┴───────────────────┴──────────────┘  │
│  ┌───────────────────────────────────────────┐  │
│  │      WBP_BottomPanel (180px)              │  │
│  │   Tabs: Import|Modeling|Media|Export      │  │
│  └───────────────────────────────────────────┘  │
└─────────────────────────────────────────────────┘
          ↓ 관리됨
┌─────────────────────────────────────────────────┐
│     TwinmotionUIManager (Subsystem)             │
│  - Panel Visibility States                      │
│  - Selected Objects                             │
│  - Thumbnail Collection                         │
│  - Event Broadcasting                           │
└─────────────────────────────────────────────────┘
```

## 설치 및 설정

### 1. C++ 코드 컴파일

```bash
# 1. Visual Studio 프로젝트 재생성
우클릭 ThirdMotion.uproject > Generate Visual Studio project files

# 2. Visual Studio에서 솔루션 열기
ThirdMotion.sln 더블클릭

# 3. 빌드
Build > Build Solution (Ctrl+Shift+B)
```

### 2. Widget Blueprint 생성

#### 자동 생성 (권장):

1. Unreal Editor 실행
2. `Tools > Execute Python Script`
3. `Content/BluePrint/UI/CreateTwinmotionUI.py` 선택
4. 실행

#### 수동 생성:

Content Browser에서 각 Widget Blueprint를 수동으로 생성:
- `WBP_TwinmotionUI`
- `WBP_TopBar`
- `WBP_LeftPanel`
- `WBP_RightPanel`
- `WBP_BottomPanel`
- `WBP_ThumbnailItem`

### 3. Parent Class 설정

각 Widget Blueprint의 Parent Class를 설정:

| Widget Blueprint | Parent Class |
|------------------|--------------|
| WBP_TwinmotionUI | TwinmotionMainWidget |
| WBP_TopBar | TwinmotionBaseWidget |
| WBP_LeftPanel | TwinmotionBaseWidget |
| WBP_RightPanel | TwinmotionBaseWidget |
| WBP_BottomPanel | TwinmotionBaseWidget |
| WBP_ThumbnailItem | TwinmotionBaseWidget |

**방법**: 각 Blueprint 열기 > File > Reparent Blueprint

## 사용 방법

### 기본 사용

#### 1. Level Blueprint에서 UI 생성

```blueprint
Event BeginPlay
└── Get Game Instance
    └── Get Subsystem (TwinmotionUIManager)
        └── Create Main UI
            └── Main UI Class: WBP_TwinmotionUI
```

#### 2. C++에서 UI 생성

```cpp
// PlayerController.h
UPROPERTY(EditDefaultsOnly, Category = "UI")
TSubclassOf<UUserWidget> MainUIClass;

// PlayerController.cpp
void AMyPlayerController::BeginPlay()
{
    Super::BeginPlay();

    UGameInstance* GI = GetGameInstance();
    if (GI)
    {
        UTwinmotionUIManager* UIManager =
            GI->GetSubsystem<UTwinmotionUIManager>();

        if (UIManager && MainUIClass)
        {
            UIManager->CreateMainUI(MainUIClass);
        }
    }
}
```

### 패널 제어

#### Blueprint에서:

```blueprint
# 좌측 패널 토글
Get UI Manager
└── Get Panel Visibility (Library)
    └── NOT
        └── Set Panel Visibility (Library, Result)

# 우측 패널 숨기기
Get UI Manager
└── Set Panel Visibility (Scene, false)
```

#### C++에서:

```cpp
UTwinmotionUIManager* UIManager =
    GetGameInstance()->GetSubsystem<UTwinmotionUIManager>();

if (UIManager)
{
    // 패널 토글
    bool bIsVisible = UIManager->IsPanelVisible(ETwinmotionPanelType::Library);
    UIManager->SetPanelVisibility(ETwinmotionPanelType::Library, !bIsVisible);
}
```

### 썸네일 관리

#### 썸네일 추가:

```cpp
// 스크린샷 캡처 후
FThumbnailData ThumbnailData;
ThumbnailData.Name = TEXT("Screenshot_001");
ThumbnailData.Thumbnail = CapturedTexture;
ThumbnailData.CaptureTime = FDateTime::Now();

UIManager->AddThumbnail(ThumbnailData);
```

#### Blueprint에서:

```blueprint
On Capture Button Clicked
└── Capture Scene to Texture 2D
    └── Make FThumbnailData
        ├── Name: "My Screenshot"
        ├── Thumbnail: [Captured Texture]
        └── Capture Time: Now
    └── Get UI Manager
        └── Add Thumbnail
```

### Scene 오브젝트 선택

```cpp
// Actor 선택
UIManager->SelectSceneObject(SelectedActor);

// 선택된 Actor 가져오기
AActor* Selected = UIManager->GetSelectedSceneObject();
```

### 이벤트 구독

```cpp
// Header
UFUNCTION()
void OnSceneObjectSelected(AActor* SelectedObject);

// Implementation
void AMyClass::BeginPlay()
{
    UIManager->OnSceneObjectSelected.AddDynamic(
        this, &AMyClass::OnSceneObjectSelected
    );
}

void AMyClass::OnSceneObjectSelected(AActor* SelectedObject)
{
    UE_LOG(LogTemp, Log, TEXT("Selected: %s"),
        *SelectedObject->GetName());
}
```

## 커스터마이징

### 색상 테마 변경

`TWINMOTION_UI_STRUCTURE.md`의 색상 팔레트 참조:

```cpp
// 예: Dark Theme
Background: #1A1A1A
Panel: #2A2A2A
Hover: #353535
Accent: #0078D4
```

각 Widget Blueprint의 Border/Button 색상을 수정하여 테마 변경 가능.

### 새로운 패널 추가

1. 새 Widget Blueprint 생성 (Parent: TwinmotionBaseWidget)
2. `TwinmotionUIManager.h`에 새 Panel Type 추가:

```cpp
UENUM(BlueprintType)
enum class ETwinmotionPanelType : uint8
{
    // ... 기존 항목들
    MyNewPanel UMETA(DisplayName = "My New Panel")
};
```

3. WBP_TwinmotionUI에 위젯 추가
4. 재컴파일

### 애니메이션 추가

각 Widget Blueprint에서:

1. Animations 탭 클릭
2. `+ Animation` 버튼
3. Timeline에서 트랙 추가
4. Event Graph에서 애니메이션 재생:

```blueprint
On Button Clicked
└── Play Animation
    └── Animation: FadeIn
    └── Start Time: 0.0
    └── Playback Speed: 1.0
```

## 문제 해결

### 컴파일 오류

**문제**: `TwinmotionUIManager를 찾을 수 없음`

**해결**:
```bash
1. Visual Studio 닫기
2. Unreal Editor 닫기
3. Binaries, Intermediate, Saved 폴더 삭제
4. .uproject 우클릭 > Generate Visual Studio project files
5. Visual Studio에서 빌드
```

### Widget이 표시되지 않음

**체크리스트**:
- [ ] `CreateMainUI()` 호출되었는지 확인
- [ ] Widget Class가 올바르게 설정되었는지 확인
- [ ] Viewport에 추가되었는지 확인
- [ ] Z-Order 확인
- [ ] Visibility가 Visible인지 확인

**디버깅**:
```cpp
if (MainUIWidget)
{
    UE_LOG(LogTemp, Log, TEXT("UI Widget Created: %s"),
        *MainUIWidget->GetName());
}
else
{
    UE_LOG(LogTemp, Error, TEXT("Failed to create UI Widget"));
}
```

### BindWidget 오류

**문제**: `BindWidget: Required widget not found`

**해결**:
1. 변수명이 Designer의 Widget 이름과 **정확히** 일치하는지 확인 (대소문자 구분)
2. `Is Variable` 체크되어 있는지 확인
3. `BindWidget` 체크되어 있는지 확인

### UIManager가 nullptr

**문제**: `GetSubsystem<UTwinmotionUIManager>()` 반환값이 null

**해결**:
```cpp
// ThirdMotion.Build.cs 확인
PublicDependencyModuleNames.AddRange(new string[] {
    "Core", "CoreUObject", "Engine", "UMG"
});

// TwinmotionUIManager.h 확인
UCLASS()
class THIRDMOTION_API UTwinmotionUIManager : public UGameInstanceSubsystem
{
    GENERATED_BODY()
    // ...
};
```

### Parent Class를 설정할 수 없음

**문제**: TwinmotionMainWidget/TwinmotionBaseWidget이 목록에 없음

**해결**:
1. C++ 코드가 성공적으로 컴파일되었는지 확인
2. Editor 재시작
3. `Tools > Refresh Visual Studio Project`
4. Hot Reload 대신 Full Recompile 시도

## 고급 기능

### 드래그 앤 드롭 구현

```cpp
// Header
virtual void NativeOnDragDetected(const FGeometry& InGeometry,
    const FPointerEvent& InMouseEvent,
    UDragDropOperation*& OutOperation) override;

// Implementation
void UMyWidget::NativeOnDragDetected(...)
{
    UDragDropOperation* DragOp = NewObject<UDragDropOperation>();
    DragOp->Payload = MyData;
    OutOperation = DragOp;
}
```

### 키보드 단축키

```cpp
virtual FReply NativeOnKeyDown(const FGeometry& InGeometry,
    const FKeyEvent& InKeyEvent) override;

FReply UMyWidget::NativeOnKeyDown(...)
{
    if (InKeyEvent.GetKey() == EKeys::F)
    {
        // F 키 처리
        return FReply::Handled();
    }
    return FReply::Unhandled();
}
```

### Scene Capture 통합

중앙 Viewport에 실시간 Scene Capture 표시:

```cpp
// ViewportContainer에 Image Widget 추가
USceneCaptureComponent2D* SceneCapture = CreateDefaultSubobject<USceneCaptureComponent2D>();
UTextureRenderTarget2D* RenderTarget = NewObject<UTextureRenderTarget2D>();

SceneCapture->TextureTarget = RenderTarget;
ImageWidget->SetBrushFromTexture(RenderTarget);
```

## 성능 최적화

### Widget Pooling

썸네일 아이템 같은 반복 위젯은 Object Pool 사용:

```cpp
TArray<UWBPThumbnailItem*> WidgetPool;

UWBPThumbnailItem* GetPooledWidget()
{
    if (WidgetPool.Num() > 0)
    {
        return WidgetPool.Pop();
    }
    return CreateWidget<UWBPThumbnailItem>(...);
}
```

### Invalidation Box 사용

변경이 적은 패널은 Invalidation Box로 감싸기:

```
Invalidation Box (Cache Relative Transforms: true)
└── [Static Content]
```

## 라이선스

이 코드는 Epic Games의 Unreal Engine 라이선스를 따릅니다.

## 기여

이슈 및 개선 사항은 프로젝트 리포지토리에 제출해주세요.

## 참고 자료

- [Unreal Engine UMG Documentation](https://docs.unrealengine.com/5.3/umg-ui-designer)
- [Twinmotion Documentation](https://www.unrealengine.com/twinmotion)
- `TWINMOTION_UI_STRUCTURE.md` - 상세 구조 문서
- `QUICK_START_GUIDE.md` - 빠른 시작 가이드

---

**버전**: 1.0
**Unreal Engine**: 5.3
**마지막 업데이트**: 2025
