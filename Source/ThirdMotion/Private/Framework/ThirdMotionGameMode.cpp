
#include "Framework/ThirdMotionGameMode.h"
#include "Framework/ThirdMotionPlayerController.h"
#include "Blueprint/UserWidget.h"
#include "Engine/GameViewportClient.h"
#include "Engine/World.h"

AThirdMotionGameMode::AThirdMotionGameMode()
{
	PlayerControllerClass = AThirdMotionPlayerController::StaticClass();

}

void AThirdMotionGameMode::BeginPlay()
{
	Super::BeginPlay();

	// 런타임에서 기본 게임 뷰포트를 숨기고 MainWidget의 ViewportContainer만 사용
	if (UGameViewportClient* ViewportClient = GetWorld()->GetGameViewport())
	{
		// 기본 뷰포트 위젯을 투명하게 설정하여 MainWidget이 전체 화면을 차지하도록 함
		// ViewportContainer 내부의 ViewportWidget이 실제 3D 렌더링을 담당
		ViewportClient->bDisableWorldRendering = false; // 월드 렌더링은 활성화 유지
	}
}
