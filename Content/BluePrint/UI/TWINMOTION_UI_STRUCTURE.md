# Twinmotion 2023.1 Style UI Structure for Unreal Engine

이 문서는 언리얼 엔진의 Widget Blueprint (UMG)를 사용하여 Twinmotion 2023.1 인터페이스를 재현하는 방법을 설명합니다.

## 전체 구조 개요

```
WBP_TwinmotionUI (Main Container)
├── Canvas Panel (Root)
│   ├── WBP_TopBar (상단 툴바)
│   ├── LeftPanel Container
│   │   └── WBP_LeftPanel (라이브러리)
│   ├── Viewport Container (중앙 뷰포트)
│   ├── RightPanel Container
│   │   └── WBP_RightPanel (Scene/Properties)
│   └── BottomPanel Container
│       └── WBP_BottomPanel (썸네일 뷰 & 탭)
```

## 1. WBP_TwinmotionUI (메인 컨테이너)

**Parent Class**: `TwinmotionMainWidget` (C++)

### 레이아웃 구조:

```
Canvas Panel (루트)
├── SizeBox (FullScreen) - Fill Screen
│   └── Overlay
│       ├── Border (Background) - #1A1A1A
│       └── Vertical Box
│           ├── WBP_TopBar (크기: Auto, 높이: 48px)
│           ├── Horizontal Box (크기: Fill)
│           │   ├── WBP_LeftPanel (너비: 280px)
│           │   ├── Spacer (2px) - #2A2A2A
│           │   ├── Border (ViewportContainer - Fill)
│           │   ├── Spacer (2px) - #2A2A2A
│           │   └── WBP_RightPanel (너비: 320px)
│           ├── Spacer (2px) - #2A2A2A
│           └── WBP_BottomPanel (크기: Auto, 높이: 180px)
```

### Blueprint 설정:

1. **Graph > Variables**:
   - `TopBar` (Widget Reference - BindWidget)
   - `LeftPanel` (Widget Reference - BindWidget)
   - `RightPanel` (Widget Reference - BindWidget)
   - `BottomPanel` (Widget Reference - BindWidget)
   - `ViewportContainer` (Border Reference - BindWidget)

2. **Event Graph**:
   - `Event Construct` → `Get Game Instance` → `Get Subsystem (TwinmotionUIManager)`

---

## 2. WBP_TopBar (상단 툴바)

**Parent Class**: `TwinmotionBaseWidget`

### 레이아웃 구조:

```
Horizontal Box (Fill)
├── Border (배경: #252525)
│   └── Horizontal Box
│       ├── Menu Bar (File, Edit, View, Help)
│       │   └── Horizontal Box
│       │       ├── Button (File) + Text
│       │       ├── Button (Edit) + Text
│       │       ├── Button (View) + Text
│       │       └── Button (Help) + Text
│       ├── Spacer (10px)
│       ├── Tool Buttons
│       │   └── Horizontal Box
│       │       ├── Button (Image: Icon_Play)
│       │       ├── Button (Image: Icon_Move)
│       │       ├── Button (Image: Icon_Rotate)
│       │       ├── Button (Image: Icon_Scale)
│       │       └── Button (Image: Icon_Camera)
│       ├── Spacer (Fill)
│       └── Right Tools
│           └── Horizontal Box
│               ├── Button (Image: Icon_Settings)
│               └── Button (Image: Icon_User)
```

### 버튼 스타일:

- **Normal**: 배경색 투명, 텍스트 #CCCCCC
- **Hovered**: 배경색 #353535, 텍스트 #FFFFFF
- **Pressed**: 배경색 #0078D4, 텍스트 #FFFFFF

---

## 3. WBP_LeftPanel (라이브러리 패널)

**Parent Class**: `TwinmotionBaseWidget`

### 레이아웃 구조:

```
Border (#2A2A2A)
└── Vertical Box
    ├── Border (헤더)
    │   └── Horizontal Box
    │       ├── Text Block ("Library")
    │       ├── Spacer (Fill)
    │       └── Button (Close/Minimize)
    ├── Search Box
    │   └── Editable Text Box (힌트: "Search library...")
    ├── Tab Buttons
    │   └── Horizontal Box
    │       ├── Button ("Objects")
    │       ├── Button ("Materials")
    │       ├── Button ("Lights")
    │       └── Button ("Effects")
    └── Scroll Box (Fill)
        └── Wrap Box (Asset Thumbnails)
            ├── WBP_LibraryItem
            ├── WBP_LibraryItem
            └── ...
```

### WBP_LibraryItem 구조:

```
Border (100x100)
└── Vertical Box
    ├── Image (Asset Thumbnail - 100x80)
    └── Text Block (Asset Name - 100x20)
```

---

## 4. WBP_RightPanel (Scene & Properties 패널)

**Parent Class**: `TwinmotionBaseWidget`

### 레이아웃 구조:

```
Border (#2A2A2A)
└── Vertical Box
    ├── Tab Switcher
    │   └── Horizontal Box
    │       ├── Button ("Scene")
    │       └── Button ("Properties")
    ├── Spacer (2px)
    └── Widget Switcher (Fill)
        ├── Scene View (Index 0)
        │   └── Vertical Box
        │       ├── Search Box
        │       └── Tree View (Scene Hierarchy)
        │           └── Scroll Box
        │               └── Scene Tree Items
        └── Properties View (Index 1)
            └── Scroll Box
                └── Vertical Box
                    ├── Property Category 1
                    │   └── Property Rows
                    ├── Property Category 2
                    │   └── Property Rows
                    └── ...
```

### Scene Tree Item 구조:

```
Horizontal Box
├── Spacer (Indent Level * 16px)
├── Button (Expand/Collapse - 16x16)
├── Image (Icon - 16x16)
├── Editable Text (Name)
└── Button (Visibility Toggle)
```

---

## 5. WBP_BottomPanel (썸네일 뷰 & 탭 패널)

**Parent Class**: `TwinmotionBaseWidget`

### 레이아웃 구조:

```
Vertical Box
├── Tab Bar
│   └── Horizontal Box
│       ├── Button ("Import")
│       ├── Button ("Modeling")
│       ├── Button ("Media")
│       └── Button ("Export")
├── Spacer (2px)
└── Widget Switcher (Fill)
    ├── Import Tab (Index 0)
    ├── Modeling Tab (Index 1)
    ├── Media Tab (Index 2) - **Thumbnail View**
    │   └── Scroll Box (Horizontal)
    │       └── Horizontal Box
    │           ├── WBP_ThumbnailItem
    │           ├── WBP_ThumbnailItem
    │           └── ...
    └── Export Tab (Index 3)
```

---

## 6. WBP_ThumbnailItem (썸네일 아이템)

**Parent Class**: `TwinmotionBaseWidget`

### 레이아웃 구조:

```
Button (160x140)
└── Border
    └── Vertical Box
        ├── Image (Thumbnail - 160x120)
        ├── Spacer (2px)
        └── Horizontal Box
            ├── Text Block (Name - Fill)
            └── Button (Delete - 20x20)
```

### Blueprint 기능:

```blueprint
Variables:
- ThumbnailData: FThumbnailData (struct)

Functions:
- SetThumbnailData(Data: FThumbnailData)
- OnThumbnailClicked() → Call UIManager->SelectThumbnail
- OnDeleteClicked() → Call UIManager->RemoveThumbnail
```

---

## C++ 클래스 구조

### 1. **TwinmotionUIManager** (GameInstanceSubsystem)

전체 UI 상태를 관리하는 싱글톤 매니저

**주요 기능**:
- Panel 가시성 관리
- 썸네일 데이터 관리
- Scene 계층 구조 관리
- 선택된 오브젝트 추적

**파일 위치**:
- `Source/ThirdMotion/TwinmotionUIManager.h`
- `Source/ThirdMotion/TwinmotionUIManager.cpp`

### 2. **TwinmotionBaseWidget** (UserWidget)

모든 위젯의 베이스 클래스

**기능**:
- UIManager 참조 자동 획득
- 초기화/정리 이벤트

**파일 위치**:
- `Source/ThirdMotion/UI/TwinmotionBaseWidget.h`
- `Source/ThirdMotion/UI/TwinmotionBaseWidget.cpp`

### 3. **TwinmotionMainWidget** (TwinmotionBaseWidget)

메인 컨테이너 위젯

**기능**:
- 모든 서브 패널 참조 관리
- Panel 토글 기능
- UIManager 이벤트 바인딩

**파일 위치**:
- `Source/ThirdMotion/UI/TwinmotionMainWidget.h`
- `Source/ThirdMotion/UI/TwinmotionMainWidget.cpp`

---

## 색상 팔레트 (Twinmotion Style)

| 용도 | 색상 코드 | 설명 |
|------|----------|------|
| Background Dark | #1A1A1A | 메인 배경 |
| Background Medium | #2A2A2A | 패널 배경 |
| Background Light | #353535 | Hover 상태 |
| Accent Blue | #0078D4 | 선택/강조 |
| Border | #3A3A3A | 구분선 |
| Text Primary | #FFFFFF | 메인 텍스트 |
| Text Secondary | #CCCCCC | 보조 텍스트 |
| Text Disabled | #666666 | 비활성 텍스트 |

---

## 사용 방법

### 1. 컴파일

```bash
# Unreal Editor에서
Tools > Refresh Visual Studio Project
Visual Studio에서 프로젝트 빌드 (Development Editor)
```

### 2. Widget Blueprint 생성

Unreal Editor에서 Python 스크립트 실행:

```
Tools > Execute Python Script
> Content/BluePrint/UI/CreateTwinmotionUI.py
```

또는 수동으로 각 Widget Blueprint 생성

### 3. Widget Blueprint 디자인

각 위젯을 열어서 위 구조대로 UI 레이아웃 구성

### 4. Blueprint에서 Parent Class 설정

- `WBP_TwinmotionUI`: Parent Class를 `TwinmotionMainWidget`으로 변경
- 나머지 Widget들: Parent Class를 `TwinmotionBaseWidget`으로 변경

### 5. 게임에서 UI 표시

Level Blueprint 또는 GameMode에서:

```blueprint
Event BeginPlay
→ Get Game Instance
→ Get Subsystem (TwinmotionUIManager)
→ Create Main UI (Class: WBP_TwinmotionUI)
```

---

## 주요 기능 구현 예제

### Panel 토글

```blueprint
# WBP_TopBar에서
On Toggle Left Panel Button Clicked
→ Get UI Manager
→ Set Panel Visibility (Library, Toggle)
```

### 썸네일 추가

```blueprint
On Capture Screenshot Button Clicked
→ Capture Scene to Texture 2D
→ Make FThumbnailData (Name, Texture, DateTime)
→ Get UI Manager
→ Add Thumbnail (ThumbnailData)
```

### Scene 오브젝트 선택

```blueprint
On Tree Item Clicked (Actor)
→ Get UI Manager
→ Select Scene Object (Actor)
→ Update Properties Panel
```

---

## 추가 커스터마이징

각 위젯은 Blueprint에서 추가 기능을 구현할 수 있습니다:

- 애니메이션 (패널 슬라이드, 페이드 등)
- 드래그 앤 드롭
- 컨텍스트 메뉴
- 키보드 단축키
- 테마 변경

---

## 문제 해결

### UIManager를 찾을 수 없음
- Game Instance Subsystem이 제대로 등록되었는지 확인
- 프로젝트가 컴파일되었는지 확인

### Widget이 표시되지 않음
- Widget을 Viewport에 추가했는지 확인
- Z-Order와 Visibility 확인
- Canvas Panel Anchors 설정 확인

### Parent Class를 설정할 수 없음
- C++ 코드가 컴파일되었는지 확인
- Editor를 재시작
- .uasset 파일 삭제 후 재생성

---

## 참고 자료

- Unreal Engine UMG Documentation
- Twinmotion User Interface Guidelines
- Material Design for Desktop Applications
