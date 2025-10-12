# Twinmotion UI - 빠른 시작 가이드

이 가이드는 Twinmotion 스타일 UI를 프로젝트에 빠르게 구현하는 방법을 설명합니다.

## 단계별 설정

### 1단계: C++ 코드 컴파일

1. Visual Studio에서 프로젝트 열기
2. Build 메뉴 > Build Solution (Ctrl+Shift+B)
3. 컴파일 완료 대기
4. Unreal Editor 실행

### 2단계: Widget Blueprint 생성

#### 옵션 A: Python 스크립트 사용 (추천)

1. Unreal Editor에서 `Tools > Execute Python Script` 선택
2. `Content/BluePrint/UI/CreateTwinmotionUI.py` 선택
3. 실행 후 Output Log에서 성공 메시지 확인

#### 옵션 B: 수동 생성

1. Content Browser에서 `Content/BluePrint/UI` 폴더로 이동
2. 우클릭 > User Interface > Widget Blueprint
3. 다음 위젯들을 생성:
   - `WBP_TwinmotionUI`
   - `WBP_TopBar`
   - `WBP_LeftPanel`
   - `WBP_RightPanel`
   - `WBP_BottomPanel`
   - `WBP_ThumbnailItem`

### 3단계: Widget Blueprint Parent Class 설정

각 Widget Blueprint를 열고 Parent Class를 변경:

#### WBP_TwinmotionUI:
1. Blueprint 열기
2. File > Reparent Blueprint
3. `TwinmotionMainWidget` 선택

#### 나머지 모든 Widget (TopBar, LeftPanel, RightPanel, BottomPanel, ThumbnailItem):
1. Blueprint 열기
2. File > Reparent Blueprint
3. `TwinmotionBaseWidget` 선택

### 4단계: WBP_TwinmotionUI 레이아웃 구성

#### 4.1 기본 구조 만들기

1. `WBP_TwinmotionUI` 열기
2. Designer 탭에서 다음 구조 생성:

```
Canvas Panel (Root)
└── SizeBox
    └── Overlay
        ├── Border (배경)
        └── Vertical Box
            ├── [TopBar 슬롯]
            ├── Horizontal Box (메인 영역)
            └── [BottomPanel 슬롯]
```

#### 4.2 상세 설정

**SizeBox:**
- Anchors: Fill Screen (0,0 to 1,1)
- Size to Content: false

**Background Border:**
- Appearance > Brush Color: #1A1A1A
- Z-Order: 0

**Vertical Box:**
- Vertical Box > Alignment: Top
- Z-Order: 1

#### 4.3 TopBar 슬롯 추가

1. Vertical Box 안에 `User Widget` 추가
2. Widget Class를 `WBP_TopBar`로 설정
3. Slot 설정:
   - Size: Auto
   - Vertical Alignment: Top
4. 변수 이름을 `TopBar`로 설정
5. `Is Variable` 체크
6. `Bind Widget` 체크 (Variable 옆의 눈 아이콘)

#### 4.4 메인 영역 (Horizontal Box) 설정

Horizontal Box에 다음 추가:

1. **Left Panel:**
   - User Widget (Class: WBP_LeftPanel)
   - 변수명: `LeftPanel`, Bind Widget
   - Slot > Size: Override, 280px

2. **Spacer (구분선):**
   - Spacer
   - Size: Override, 2px

3. **Viewport Container:**
   - Border
   - 변수명: `ViewportContainer`, Bind Widget
   - Slot > Size: Fill
   - Brush Color: #000000

4. **Spacer (구분선):**
   - Spacer
   - Size: Override, 2px

5. **Right Panel:**
   - User Widget (Class: WBP_RightPanel)
   - 변수명: `RightPanel`, Bind Widget
   - Slot > Size: Override, 320px

#### 4.5 BottomPanel 슬롯 추가

1. Vertical Box 하단에 User Widget 추가
2. Widget Class를 `WBP_BottomPanel`로 설정
3. 변수명: `BottomPanel`, Bind Widget
4. Slot 설정:
   - Size: Override, 180px

### 5단계: WBP_TopBar 디자인

1. `WBP_TopBar` 열기
2. Designer에서 구조 생성:

```
Border (배경: #252525, 높이: 48px)
└── Horizontal Box
    ├── Menu Buttons
    │   ├── Button (Text: "File")
    │   ├── Button (Text: "Edit")
    │   ├── Button (Text: "View")
    │   └── Button (Text: "Help")
    ├── Spacer (10px)
    └── Tool Buttons
        ├── Button (Icon: Play)
        ├── Button (Icon: Move)
        └── ...
```

**버튼 스타일:**
- Normal: Background Color Transparent, Text #CCCCCC
- Hovered: Background Color #353535
- Pressed: Background Color #0078D4

### 6단계: WBP_LeftPanel 디자인

```
Border (#2A2A2A)
└── Vertical Box
    ├── Border (Header, 높이: 40px)
    │   └── Text Block ("Library")
    ├── Editable Text Box (Search, Hint: "Search library...")
    ├── Horizontal Box (탭)
    │   ├── Button ("Objects")
    │   ├── Button ("Materials")
    │   └── ...
    └── Scroll Box (Fill)
        └── Wrap Box
            └── [라이브러리 아이템들]
```

### 7단계: WBP_RightPanel 디자인

```
Border (#2A2A2A)
└── Vertical Box
    ├── Horizontal Box (탭 스위처)
    │   ├── Button ("Scene")
    │   └── Button ("Properties")
    └── Widget Switcher
        ├── Scroll Box (Scene View)
        └── Scroll Box (Properties View)
```

### 8단계: WBP_BottomPanel 디자인

```
Vertical Box
├── Horizontal Box (탭 바)
│   ├── Button ("Import")
│   ├── Button ("Modeling")
│   ├── Button ("Media")
│   └── Button ("Export")
└── Widget Switcher
    ├── [Import Content]
    ├── [Modeling Content]
    ├── Scroll Box (Media - Thumbnails)
    │   └── Horizontal Box
    │       └── [WBP_ThumbnailItem들]
    └── [Export Content]
```

### 9단계: WBP_ThumbnailItem 디자인

```
Button (160x140)
└── Border
    └── Vertical Box
        ├── Image (160x120) - Thumbnail
        └── Horizontal Box
            ├── Text Block (Name)
            └── Button (Delete, 20x20)
```

### 10단계: Event Graph 설정

#### WBP_TwinmotionUI Event Graph:

```blueprint
Event Construct
├── Parent: Construct
└── Get UI Manager
    └── Is Valid?
        └── True → (초기화 완료)
```

#### WBP_TopBar Event Graph:

각 버튼에 대한 Click 이벤트 추가:

```blueprint
On Toggle Left Panel Clicked
└── Get UI Manager
    └── Get Panel Visibility (Library)
        └── NOT
            └── Set Panel Visibility (Library, Result)
```

### 11단계: Game Mode에서 UI 생성

Level Blueprint 또는 Player Controller에서:

```blueprint
Event Begin Play
└── Get Game Instance
    └── Get Subsystem (TwinmotionUIManager)
        └── Create Main UI
            └── Class: WBP_TwinmotionUI
            └── Add to Viewport
```

또는 C++에서:

```cpp
void AMyPlayerController::BeginPlay()
{
    Super::BeginPlay();

    UGameInstance* GI = GetGameInstance();
    if (GI)
    {
        UTwinmotionUIManager* UIManager = GI->GetSubsystem<UTwinmotionUIManager>();
        if (UIManager && MainUIClass)
        {
            UIManager->CreateMainUI(MainUIClass);
        }
    }
}
```

### 12단계: 테스트

1. PIE (Play In Editor) 실행
2. UI가 표시되는지 확인
3. 패널 토글 버튼 테스트
4. 레이아웃 확인

## 자주 하는 실수

### Widget이 표시되지 않음
- Widget이 Viewport에 추가되었는지 확인
- Z-Order 확인
- Visibility가 Visible인지 확인

### Parent Class를 찾을 수 없음
- C++ 코드가 컴파일되었는지 확인
- Editor 재시작
- `Tools > Refresh Visual Studio Project`

### BindWidget 오류
- 변수명이 정확히 일치하는지 확인 (대소문자 구분)
- `Is Variable`과 `BindWidget`이 모두 체크되었는지 확인

### UIManager가 null
- Game Instance Subsystem이 등록되었는지 확인
- `ThirdMotion.Build.cs`에 필요한 모듈이 추가되었는지 확인

## 다음 단계

- 각 패널에 실제 기능 구현
- 애니메이션 추가 (슬라이드, 페이드 등)
- 키보드 단축키 추가
- 드래그 앤 드롭 기능 구현
- 테마 시스템 추가

## 추가 리소스

- `TWINMOTION_UI_STRUCTURE.md` - 전체 구조 상세 문서
- Unreal Engine UMG 문서
- Twinmotion 공식 문서
