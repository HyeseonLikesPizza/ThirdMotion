
#include "UI/Widget/MainWidget.h"

#include "Components/CanvasPanelSlot.h"
#include "Components/Widget.h"
#include "UI/Widget/ViewportWidget.h"
#include "Blueprint/WidgetTree.h"
#include "Camera/CameraActor.h"

void UMainWidget::NativeConstruct()
{
	Super::NativeConstruct();

	// Setup 3D viewport
	SetupViewport();

}

void UMainWidget::NativeDestruct()
{
	Super::NativeDestruct();
}

void UMainWidget::SetupViewport()
{
	// 카메라 액터 생성
	if (UWorld* World = GetWorld())
	{
		// 뷰포트용 카메라 생성
		ACameraActor* ViewportCamera = World->SpawnActor<ACameraActor>();
		ViewportCamera->SetActorLocation(FVector(-500, 0, 200));
		ViewportCamera->SetActorRotation(FRotator(-20, 0, 0));
        
		// Player Controller의 뷰 타겟 설정
		if (APlayerController* PC = World->GetFirstPlayerController())
		{
			// 뷰포트 영역에서만 렌더링하도록 설정
			PC->SetViewTarget(ViewportCamera);
		}
	}
}
