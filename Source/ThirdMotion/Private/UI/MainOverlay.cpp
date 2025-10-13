// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/MainOverlay.h"

#include "SocketSubsystem.h"
#include "Components/VerticalBox.h"
#include "Components/Button.h"
#include "Components/CanvasPanel.h"
#include "Components/EditableTextBox.h"
#include "Components/WidgetSwitcher.h"
#include "Kismet/GameplayStatics.h"


void UMainOverlay::NativeConstruct()
{
	Super::NativeConstruct();

}

void UMainOverlay::NativeOnInitialized()
{
	Super::NativeOnInitialized();

	if (HostBtn)
	{
		HostBtn->OnClicked.AddDynamic(this, &UMainOverlay::CreateHost);
	}

	if (JoinBtn)
	{
		JoinBtn->OnClicked.AddDynamic(this, &UMainOverlay::GoToJoinPage);
	}

	if (CancelBtn)
	{
		CancelBtn->OnClicked.AddDynamic(this, &UMainOverlay::GoToFirstPage);
	}

	if (JoinConfirmBtn)
	{
		JoinConfirmBtn->OnClicked.AddDynamic(this, &UMainOverlay::OnJoinConfirmClicked);
	}
	
	// 네트워크 실패 델리게이트 바인딩
	if (GetWorld() && GetWorld()->GetFirstPlayerController())
	{
		GEngine->OnNetworkFailure().AddUObject(this, &UMainOverlay::HandleNetworkFailure);
	}

	// 🔹 마우스 커서 활성화
	if (APlayerController* PC = GetWorld()->GetFirstPlayerController())
	{
		PC->bShowMouseCursor = true;
		PC->SetInputMode(FInputModeUIOnly());
		PC->SetIgnoreLookInput(true);  // (선택) 마우스 움직여도 카메라 안움직이게
		PC->SetIgnoreMoveInput(true);  // (선택) WASD 등 막기
		PC->SetInputMode(FInputModeUIOnly());
	}
}




//Host버튼 눌렸을 때

void UMainOverlay::CreateHost()
{
	UE_LOG(LogTemp, Warning, TEXT("UMainOverlay::CreateHost() - Host Button Clicked!")); // Diagnostic log

	bool bCanBindAll;
	TSharedPtr<FInternetAddr> LocalAddr = ISocketSubsystem::Get(PLATFORM_SOCKETSUBSYSTEM)->GetLocalHostAddr(*GLog, bCanBindAll);
    
	url = LocalAddr.IsValid() ? LocalAddr->ToString(false) : "127.0.0.1";
	
	UE_LOG(LogTemp, Display, TEXT("Host URL : %s"), *url);

	// ✅ Viewport에서 제거
	RemoveFromParent();
	
	UGameplayStatics::OpenLevel(this, FName("MainMap"), true, "listen");


}



//Cancel버튼 눌렸을 때
void UMainOverlay::GoToFirstPage()
{
	if (WidgetSwitcher)
	{
		WidgetSwitcher->SetActiveWidgetIndex(0);
	}
}

//Join버튼 눌렸을 때
void UMainOverlay::GoToJoinPage()
{
	if (WidgetSwitcher)
	{
		WidgetSwitcher->SetActiveWidgetIndex(1);
	}
}

//Join페이지의 Confirm버튼 눌렸을때
void UMainOverlay::OnJoinConfirmClicked()
{
	if (!UrlInputBox)  return;

	FText Text = UrlInputBox->GetText();
	url = Text.ToString();

	// URL/IP 검증 (간단 예시)
	if (url.IsEmpty())
	{
		UE_LOG(LogTemp, Warning, TEXT("IP/URL이 비어있음"))
		return;
	}

	//RemoveFromParent();

	//서버로 이동
	UGameplayStatics::OpenLevel(this, *url, true);
}

void UMainOverlay::HandleNetworkFailure(UWorld* World, UNetDriver* NetDriver, ENetworkFailure::Type FailureType,
	const FString& ErrorString)
{
	// 서버 접속 실패 시 메시지 출력
	UE_LOG(LogTemp, Error, TEXT("네트워크 실패: %s"), *ErrorString);

	// UI에 팝업 메시지 띄우기 가능
	// 예: UWidgetBlueprintLibrary::Create(UserWidget, ...) + AddToViewport()
}
