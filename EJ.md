# CLAUDE.md

This file provides guidance to Claude Code (claude.ai/code) when working with code in this repository.

## 프로젝트 개요

**ThirdMotion**은 Twinmotion 스타일의 3D 편집 애플리케이션을 구현한 Unreal Engine 5.6 프로젝트입니다. 핵심 로직은 C++로, UI 구현은 Blueprint로 작성되었습니다.

## 빌드 명령어

### 완전 클린 리빌드
```batch
cd C:\Users\user\Desktop\Fork\ThirdMotion_share
rmdir /s /q Intermediate
rmdir /s /q Binaries
rmdir /s /q .vs
```

이후 `ThirdMotion.uproject` 우클릭 → **Generate Visual Studio project files**

### Visual Studio 빌드
`ThirdMotion.sln` 열어서 빌드하거나 커맨드라인 사용:
```batch
"C:\Program Files\Epic Games\UE_5.3\Engine\Build\BatchFiles\Build.bat" ThirdMotionEditor Win64 Development -Project="C:\Users\user\Desktop\Fork\ThirdMotion_share\ThirdMotion.uproject" -WaitMutex -FromMsBuild
```

### Hot Reload 이슈
**중요**: Hot Reload는 위젯 타입 변경(예: `UUserWidget*` → `URightPanel*`)에서 제대로 작동하지 않습니다. 다음 경우 항상 완전 클린 리빌드 필요:
- 헤더의 위젯 프로퍼티 타입 변경
- `BindWidget` 프로퍼티 추가/제거
- 클래스 상속 계층 변경

`Intermediate/`와 `Binaries/` 폴더 삭제, 프로젝트 파일 재생성, 처음부터 리빌드하세요.

## 아키텍처

### UI 프레임워크 (MVC/Observer 패턴)

위젯, 컨트롤러, 데이터 간 관심사를 분리한 커스텀 UI 아키텍처:

#### 위젯 계층 구조
```
UBaseWidget (모든 UI 위젯의 추상 베이스)
├── UMainWidget (메인 컨테이너 - 전체 화면 UI)
├── ULibraryItem (에셋 라이브러리 아이템 위젯)
├── USceneTreeItem (씬 계층 구조 트리 아이템)
├── URightPanel (씬 패널 컨테이너)
├── UBottomBar (하단 툴바)
├── UTopBar (상단 메뉴바)
└── UViewportWidget (SViewport를 사용한 임베디드 3D 뷰포트)
```

**핵심 패턴**: 모든 커스텀 위젯은 `UBaseWidget`을 상속합니다 (`UUserWidget` 직접 상속 X). 이를 통해 제공되는 것:
- 일관된 라이프사이클 관리 (NativeConstruct/NativeDestruct)
- 공통 유틸리티 함수
- 프로젝트별 기능

#### WidgetController 패턴
```
UBaseWidgetController (추상 베이스)
├── UScenePanelController (씬 계층 구조 로직)
└── ULibraryWidgetController (에셋 라이브러리 로직)
```

**목적**: UI 표현(위젯)과 비즈니스 로직(컨트롤러)을 분리. 컨트롤러가 처리하는 것:
- 데이터 가져오기 및 처리
- 이벤트 브로드캐스팅 (Observer 패턴)
- 상태 관리
- 위젯 직접 조작 없음

#### Observer 패턴 구현
이벤트 주도 통신을 위해 Unreal의 Dynamic Multicast Delegate 사용:

**예시 흐름**:
1. `UBottomBar` 버튼 클릭 → `OnSceneButtonClicked` 브로드캐스트
2. `UMainWidget`이 이벤트 구독 → 패널 표시/숨김
3. `USceneTreeItem` 상태 변경 → `UScenePanelController`로 브로드캐스트
4. `UScenePanelController`가 씬 업데이트 → `OnSceneTreeRefreshed` 브로드캐스트
5. `URightPanel`이 업데이트 수신 → UI 새로고침

**바인딩 패턴**:
```cpp
// 위젯 NativeConstruct에서:
BottomBar->OnSceneButtonClicked.AddDynamic(this, &UMainWidget::OnSceneButtonClicked);

// 위젯 NativeDestruct에서 (필수):
BottomBar->OnSceneButtonClicked.RemoveDynamic(this, &UMainWidget::OnSceneButtonClicked);
```

### 네트워크 아키텍처 (Command 패턴)

`UServerManager` (GameInstance Subsystem)가 네트워크 작업을 위한 Command 패턴 구현:

```
UNetworkCommand (추상 베이스)
├── UCreateServerCommand (게임 호스트)
├── UConnectToServerCommand (게임 참가)
└── UDisconnectCommand (게임 나가기)
```

**네트워크 역할 감지**:
```cpp
bool IsServer() = GetAuthGameMode() != nullptr
bool IsClient() = GetNetMode() == NM_Client
bool HasAuthority() = Standalone || ListenServer || DedicatedServer
```

### 씬 관리

`USceneManager` (World Subsystem)가 처리:
- GameplayTag를 통한 액터 스폰
- 씬 상태 관리
- 비동기 에셋 로딩을 위한 `AssetResolver` 통합

### 저장/로드 시스템 (Memento 패턴)

```
FActorSaveData → 개별 액터 상태 스냅샷
FSceneSaveData → 전체 씬 스냅샷
UThirdMotionSaveGame → 영구 저장소
USaveGameManager (GameInstance Subsystem) → 저장/로드 오케스트레이션
```

**Save As 기능**:
```cpp
SaveGameManager->SaveSceneAs("CustomName", "SceneName");
```

### 뷰포트 통합

`UViewportWidget`이 Slate의 `SViewport`를 사용하여 3D 월드 뷰를 UI에 임베드:

**구현**:
- `RebuildWidget()`이 `SOverlay`로 래핑된 `SViewport` 생성
- 커스텀 `SceneViewport`와 함께 기존 `GameViewportClient` 사용
- `MainWidget::SetupViewport()`에서 카메라 설정 - 전용 카메라 액터 생성
- `ViewportContainer` (UBorder) 위젯 안에 3D 월드 렌더링

**주요 파일**:
- `ViewportWidget.h/cpp` - SViewport 래퍼 위젯
- `MainWidget.cpp:SetupViewport()` - 카메라 초기화

## 코드 구조

### 디렉토리 구조
```
Source/ThirdMotion/
├── Public/
│   ├── Framework/         # GameMode, GameInstance, PlayerController
│   ├── UI/
│   │   ├── Widget/        # 모든 위젯 클래스
│   │   ├── Panel/         # 패널 컨테이너 위젯
│   │   └── WidgetController/  # MVC 컨트롤러
│   ├── Network/           # ServerManager, NetworkCommands
│   ├── Save/              # SaveGame 시스템
│   ├── Edit/              # SceneManager, AssetResolver
│   └── Data/              # 데이터 구조 (ActorSaveData 등)
└── Private/               # 구현 파일 (Public/ 미러링)
```

### BindWidget 요구사항

`meta = (BindWidget)` 사용 시:
1. **정확한 이름 일치** 필수 - C++ 프로퍼티와 Blueprint 위젯 이름
2. **타입이 일치**해야 함 (옵셔널 위젯은 `BindWidgetOptional` 사용)
3. 프로퍼티는 `UPROPERTY(BlueprintReadWrite, meta = (BindWidget))` 형식

예시:
```cpp
// C++에서:
UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
UBottomBar* BottomBar;  // Blueprint에서 이름이 정확히 "BottomBar"여야 함

UPROPERTY(BlueprintReadWrite, meta = (BindWidgetOptional))
URightPanel* RightPanel;  // 옵셔널 - 없어도 크래시 안 남
```

### 공통 구조체 정의

`FSceneTreeItemData` - `SceneTreeItem.h`에 정의되어 있으며 다음에서 사용:
- `ScenePanelController` (씬 계층 구조 관리)
- `SceneTreeItem` (개별 트리 노드)
- `RightPanel` (트리 표시)

**절대** 이 구조체를 재정의하지 마세요 - 항상 `UI/Widget/SceneTreeItem.h` include 하세요

## 모듈 의존성

`ThirdMotion.Build.cs`에서:
- **Public**: Core, CoreUObject, Engine, InputCore, EnhancedInput, UMG, MoviePlayer, Sockets, Networking, GameplayTags, NetCore
- **Private**: Slate, SlateCore, ImageWrapper

## PlayerController UI 생성 흐름

**올바른 접근법** (현재 구현):
1. `ThirdMotionGameMode`가 생성자에서 `PlayerControllerClass` 설정
2. `ThirdMotionPlayerController::BeginPlay()`가 MainMap 레벨 감지
3. 0.2초 타이머 후, `CreateMainFormWidget()`이 UI 생성
4. (Blueprint에서 설정된) `MainFormWidgetClass` 스폰되어 뷰포트에 추가

**안티패턴**: GameMode에서 UI 생성하지 마세요 - 멀티플레이어 문제 발생 (원격 클라이언트가 UI를 받음).

## 흔한 실수

1. **NativeDestruct 정리 누락** - 크래시 방지를 위해 항상 델리게이트 언바인드
2. **Hot Reload 후 타입 불일치** - 위젯 타입 변경 시 전체 리빌드 필요
3. **BindWidget 이름 불일치** - Blueprint 위젯 이름이 C++ 프로퍼티 이름과 정확히 일치해야 함
4. **중복 구조체 정의** - forward declaration과 include 사용, 절대 재정의하지 마세요
5. **GameMode에서 UI 생성** - 멀티플레이어 호환성을 위해 항상 PlayerController에서 UI 생성

## 개발 워크플로우

1. **Blueprint 업데이트가 필요한 C++ 변경**:
   - Unreal Editor 닫기
   - Intermediate/Binaries 삭제
   - C++ 프로젝트 리빌드
   - Editor 열기 (Blueprint 바이트코드 재생성됨)
   - 타입 변경이 있었다면 Blueprint 업데이트

2. **새 위젯 추가**:
   - `UBaseWidget`을 상속하는 C++ 클래스 생성
   - NativeConstruct/NativeDestruct 구현
   - Blueprint 위젯 생성 (WBP_*)
   - 부모 클래스를 C++ 클래스로 설정
   - BindWidget 프로퍼티와 정확히 일치하는 이름으로 UI 요소 추가

3. **WidgetController 추가**:
   - `UBaseWidgetController` 상속
   - `Init()` 오버라이드
   - 이벤트용 델리게이트 정의
   - 컨트롤러 참조를 통해 위젯의 NativeConstruct에서 위젯에 연결
