# Twinmotion UI - Blueprint 예제 모음

이 문서는 Twinmotion UI 시스템의 다양한 Blueprint 구현 예제를 제공합니다.

## 목차

1. [기본 설정](#기본-설정)
2. [패널 제어](#패널-제어)
3. [Scene 계층 구조](#scene-계층-구조)
4. [Library 관리](#library-관리)
5. [썸네일 시스템](#썸네일-시스템)
6. [키보드 단축키](#키보드-단축키)
7. [드래그 앤 드롭](#드래그-앤-드롭)

---

## 기본 설정

### 1. Level Blueprint에서 UI 초기화

```
Event Begin Play
├── Get Game Instance
│   └── Get Subsystem (TwinmotionUIManager)
│       ├── Is Valid? → [TRUE]
│       │   └── Create Main UI
│       │       └── Main UI Class: WBP_TwinmotionUI
│       └── Is Valid? → [FALSE]
│           └── Print String: "Failed to get UI Manager"
└── Get Player Controller (0)
    └── Set Show Mouse Cursor: TRUE
    └── Set Input Mode UI Only
        └── Widget to Focus: MainUIWidget
```

### 2. Player Controller에서 UI 생성

**Header (C++)**:
```cpp
UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "UI")
TSubclassOf<UUserWidget> TwinmotionUIClass;

UPROPERTY()
UTwinmotionUIManager* UIManager;
```

**BeginPlay Blueprint**:
```
Event Begin Play
├── Parent: Begin Play
└── Get Game Instance
    └── Get Subsystem (TwinmotionUIManager)
        └── Set UI Manager (Variable)
        └── Create Main UI
            └── Class: Twinmotion UI Class
```

---

## 패널 제어

### 1. 좌측 패널 토글 버튼 (WBP_TopBar)

```
On Toggle Left Panel Button Clicked
└── Get UI Manager
    └── Branch
        ├── Condition: Is Valid
        ├── TRUE:
        │   └── Get Panel Visibility
        │       └── Panel Type: Library
        │       └── NOT (Boolean)
        │           └── Set Panel Visibility
        │               ├── Panel Type: Library
        │               └── Visible: [Result from NOT]
        └── FALSE:
            └── Print String: "UI Manager not found"
```

### 2. 모든 패널 숨기기/표시 (전체화면 모드)

```
On Fullscreen Button Clicked
└── Get UI Manager
    └── Sequence
        ├── 0: Set Panel Visibility (Library, false)
        ├── 1: Set Panel Visibility (Scene, false)
        ├── 2: Set Panel Visibility (Properties, false)
        └── 3: Set Panel Visibility (Thumbnail, false)
```

### 3. 패널 가시성 상태 저장/복원

**변수**:
- `SavedPanelStates` (Map: ETwinmotionPanelType → Boolean)

**저장**:
```
On Save Layout Button Clicked
└── Get UI Manager
    └── Sequence
        ├── 0: Get Panel Visibility (Library)
        │   └── Map Add (SavedPanelStates)
        │       ├── Key: Library
        │       └── Value: [Result]
        ├── 1: Get Panel Visibility (Scene)
        │   └── Map Add (SavedPanelStates)
        └── 2: ... (반복)
```

**복원**:
```
On Restore Layout Button Clicked
└── For Each Loop (SavedPanelStates)
    └── Set Panel Visibility
        ├── Panel Type: [Loop Key]
        └── Visible: [Loop Value]
```

---

## Scene 계층 구조

### 1. Scene 계층 구조 빌드 (WBP_RightPanel)

```
Event Construct
└── Get UI Manager
    └── Bind Event to On Scene Object Selected
        └── Update Scene Hierarchy

Custom Event: Update Scene Hierarchy
└── Clear Children (SceneTreeScrollBox)
└── Get All Actors Of Class (Actor)
    └── For Each Loop
        ├── Create Scene Tree Item Data
        │   ├── Name: Get Display Name
        │   ├── Actor: [Loop Item]
        │   ├── Indent Level: 0
        │   ├── Is Visible: Is Hidden In Game (NOT)
        │   └── Has Children: Get Num Children Components > 0
        └── Create Widget (WBP_SceneTreeItem)
            └── Set Scene Tree Item Data: [Data]
            └── Add Child to Vertical Box (SceneTreeScrollBox)
```

### 2. Scene Tree Item 선택 처리

**WBP_SceneTreeItem Event Graph**:
```
On Select Button Clicked
└── Get UI Manager
    └── Select Scene Object
        └── Actor: [ItemData.Actor]
```

### 3. Scene Object 필터링

**WBP_RightPanel**:
```
On Search Text Changed (Text)
└── Set Search Filter Text (Variable)
└── Update Scene Hierarchy (Custom Event)
    └── Get All Actors Of Class
        └── For Each Loop
            └── Branch
                ├── Condition: Does String Contain
                │   ├── Search In: Get Display Name
                │   └── Substring: Search Filter Text
                ├── TRUE: Add to Tree
                └── FALSE: Skip
```

---

## Library 관리

### 1. Library Assets 로드 (WBP_LeftPanel)

```
Event Construct
└── Load Library Assets (Custom Event)

Custom Event: Load Library Assets
└── Get Object Library
    └── Get All Blueprint Classes In Path
        └── Path: "/Game/Assets/Library"
        └── For Each Loop
            ├── Create Library Item Data
            │   ├── Name: Get Class Display Name
            │   ├── Path: Get Path Name
            │   ├── Type: Object
            │   └── Thumbnail: Load Texture (Path + "_Icon")
            └── Create Widget (WBP_LibraryItem)
                └── Set Library Item Data
                └── Add Child to Wrap Box
```

### 2. Library 검색 기능

```
On Search Text Changed (Text)
└── Get UI Manager
    └── Filter Library Assets
        └── Search Term: [Text]
    └── Refresh Library Display (Custom Event)

Custom Event: Refresh Library Display
└── Clear Children (LibraryWrapBox)
└── Get All Library Items (from cache)
    └── For Each Loop
        └── Branch
            ├── Condition: Name Contains (Search Term)
            ├── TRUE: Add Widget to Wrap Box
            └── FALSE: Skip
```

### 3. Library Item 드래그 시작

**WBP_LibraryItem**:
```
Override: On Mouse Button Down
└── Detect Drag If Pressed
    └── Drag Key: Left Mouse Button

Override: On Drag Detected
└── Create Drag Drop Operation
    ├── Operation Class: LibraryItemDragDropOperation
    ├── Payload: Library Item Data
    └── Default Drag Visual: Create Widget (WBP_DragPreview)
        └── Set Thumbnail: ItemData.Thumbnail
```

---

## 썸네일 시스템

### 1. 스크린샷 캡처 및 썸네일 추가

```
On Capture Screenshot Button Clicked
└── Sequence
    ├── 0: Hide UI (Set Visibility: Collapsed)
    ├── 1: Delay (0.1 seconds)
    ├── 2: Capture Scene to Render Target
    │   └── Render Target: Create Render Target 2D
    │       ├── Width: 1920
    │       ├── Height: 1080
    │       └── Format: RTF_RGBA8
    ├── 3: Show UI (Set Visibility: Visible)
    ├── 4: Create Thumbnail Data
    │   ├── Name: "Screenshot_" + Now (Format: HH:mm:ss)
    │   ├── Thumbnail: Render Target → Texture 2D
    │   └── Capture Time: Now
    └── 5: Get UI Manager
        └── Add Thumbnail (Thumbnail Data)
```

### 2. 썸네일 목록 업데이트 (WBP_BottomPanel)

```
Event Construct
└── Get UI Manager
    └── Bind Event to On Thumbnails Updated
        └── Refresh Thumbnail List (Custom Event)

Custom Event: Refresh Thumbnail List
└── Clear Children (ThumbnailScrollBox)
└── Get UI Manager
    └── Get Thumbnails
        └── For Each Loop
            └── Create Widget (WBP_ThumbnailItem)
                └── Set Thumbnail Data: [Loop Item]
                └── Bind Event to On Delete Clicked
                    └── Remove Thumbnail (Index: Loop Index)
                └── Add Child to Horizontal Box
```

### 3. 썸네일 클릭 → 큰 이미지 보기

**WBP_ThumbnailItem**:
```
On Thumbnail Button Clicked
└── Create Widget (WBP_ImageViewer)
    └── Set Image: Thumbnail Data.Thumbnail
    └── Add to Viewport
```

**WBP_ImageViewer**:
```
Structure:
Overlay
├── Border (Background, Semi-Transparent Black)
│   └── Button (Close on Click)
└── Image (Thumbnail, Center)
    └── Size: Fit to Screen

On Background Button Clicked
└── Remove from Parent
```

---

## 키보드 단축키

### 1. WBP_TwinmotionUI에서 키보드 처리

```
Override: On Key Down (Key Event)
└── Switch on Key
    ├── Case: F (Key)
    │   └── Toggle Left Panel
    │   └── Return: Handled
    ├── Case: G (Key)
    │   └── Toggle Right Panel
    │   └── Return: Handled
    ├── Case: H (Key)
    │   └── Toggle Bottom Panel
    │   └── Return: Handled
    ├── Case: F11 (Key)
    │   └── Toggle Fullscreen Mode
    │   └── Return: Handled
    ├── Case: Escape (Key)
    │   └── Deselect All
    │   └── Return: Handled
    └── Default:
        └── Return: Unhandled
```

### 2. Scene Object 단축키 (Delete, Rename)

**WBP_SceneTreeItem**:
```
Override: On Key Down (Key Event)
└── Branch
    ├── Condition: Is Selected
    ├── TRUE:
    │   └── Switch on Key
    │       ├── Case: Delete (Key)
    │       │   └── Destroy Actor (ItemData.Actor)
    │       │   └── Return: Handled
    │       ├── Case: F2 (Key)
    │       │   └── Name Text → Set Keyboard Focus
    │       │   └── Return: Handled
    │       └── Default: Return Unhandled
    └── FALSE:
        └── Return: Unhandled
```

---

## 드래그 앤 드롭

### 1. Drag Drop Operation 클래스 생성

**Blueprint Class**: `BP_LibraryItemDragDrop` (Parent: DragDropOperation)

**Variables**:
- `DraggedItemData` (FLibraryItemData)
- `DragVisual` (Widget Reference)

### 2. Viewport에 Drop 처리

**ViewportContainer (WBP_TwinmotionUI)**:
```
Override: On Drop (Geometry, Pointer Event, Operation)
└── Cast to BP_LibraryItemDragDrop
    └── Branch (Is Valid)
        ├── TRUE:
        │   └── Spawn Actor from Library Item
        │       ├── Class: Get Class from Path (DraggedItemData.Path)
        │       ├── Location: Trace from Mouse Position
        │       │   └── Line Trace for Objects
        │       │       ├── Start: Camera Location
        │       │       └── End: Camera Location + (Mouse Direction * 10000)
        │       └── Rotation: (0, 0, 0)
        │   └── Get UI Manager
        │       └── Add to Scene Hierarchy
        │   └── Return: TRUE
        └── FALSE:
            └── Return: FALSE
```

### 3. Scene Tree Item 재정렬 (Drop on Tree Item)

**WBP_SceneTreeItem**:
```
Override: On Drop
└── Cast to BP_SceneTreeDragDrop
    └── Branch (Is Valid)
        ├── TRUE:
        │   └── Attach Actor to Actor
        │       ├── Target: ItemData.Actor
        │       ├── Parent: DraggedItemData.Actor
        │       └── Socket Name: None
        │   └── Refresh Scene Tree
        │   └── Return: TRUE
        └── FALSE:
            └── Return: FALSE
```

---

## 고급 예제

### 1. 탭 전환 애니메이션 (WBP_BottomPanel)

**Animation**: `TabSwitchAnim`
- Track: Widget Switcher → Active Widget Index (0 → 1)
- Duration: 0.3s
- Easing: Ease In Out

**Blueprint**:
```
On Import Tab Button Clicked
└── Sequence
    ├── 0: Set Active Widget Index (0)
    ├── 1: Play Animation (TabSwitchAnim)
    └── 2: Get UI Manager
        └── Set Active Bottom Tab (Import)
```

### 2. Properties Panel 동적 생성

**WBP_RightPanel - Properties View**:
```
On Scene Object Selected (Actor)
└── Clear Children (PropertiesVerticalBox)
└── Get All Properties (Actor)
    └── For Each Loop (Property)
        └── Switch on Property Type
            ├── Case: Float
            │   └── Create Widget (WBP_PropertyFloat)
            │       └── Set Property Name and Value
            ├── Case: Vector
            │   └── Create Widget (WBP_PropertyVector)
            ├── Case: Bool
            │   └── Create Widget (WBP_PropertyBool)
            └── Default:
                └── Create Widget (WBP_PropertyText)
        └── Add Child to Vertical Box
```

### 3. 자동 저장 시스템

```
Event Construct
└── Set Timer by Event
    ├── Event: Auto Save
    ├── Time: 300.0 (5 minutes)
    └── Looping: TRUE

Custom Event: Auto Save
└── Sequence
    ├── 0: Get UI Manager
    │   └── Get Thumbnails
    │       └── Save Thumbnails to Disk
    ├── 1: Save Panel Layout
    └── 2: Print String: "Auto-saved at " + Now
```

---

## 팁 & 트릭

### 최적화

1. **Widget Pooling 사용**:
```
Variable: WidgetPool (Array of WBP_ThumbnailItem)

Get Pooled Widget:
└── Branch
    ├── Condition: Pool Size > 0
    ├── TRUE: Pop from Array
    └── FALSE: Create Widget
```

2. **Invalidation Box 활용**:
- 정적 콘텐츠를 Invalidation Box로 감싸기
- `Can Cache: true`, `Invalidation Root Mode: Auto`

3. **Lazy Loading**:
```
On Tab Switched (Index)
└── Branch
    ├── Condition: Is Content Loaded (Index)
    ├── TRUE: Show Content
    └── FALSE: Load Content → Show Content
```

### 디버깅

1. **UI Manager 상태 확인**:
```
Custom Event: Debug UI State
└── Get UI Manager
    └── Sequence
        ├── Print: "Thumbnail Count: " + Get Thumbnail Count
        ├── Print: "Selected Object: " + Get Selected Scene Object
        └── Print: "Panel States: " + Get All Panel Visibility
```

2. **Performance Monitoring**:
```
Event Tick
└── Get Widget Tick Frequency
    └── Branch
        ├── Condition: Delta Time > 0.033 (30 FPS)
        └── TRUE: Print Warning: "UI Performance Issue"
```

---

이 예제들을 참고하여 자신만의 Twinmotion 스타일 UI를 구축하세요!
