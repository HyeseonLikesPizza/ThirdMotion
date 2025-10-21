// Fill out your copyright notice in the Description page of Project Settings.


#include "Framework/ThirdMotionGameInstance.h"

#include "MoviePlayer.h"
#include "MoviePlayer.h"
#include "Blueprint/UserWidget.h"


void UThirdMotionGameInstance::Init()
{
	Super::Init();

	// 맵 로드 콜백 바인딩
	FCoreUObjectDelegates::PreLoadMap.AddUObject(this, &UThirdMotionGameInstance::BeginLoadingScreen);
	FCoreUObjectDelegates::PreLoadMapWithContext.AddUObject(this, &UThirdMotionGameInstance::BeginLoadingScreen_WithContext);
	
}

void UThirdMotionGameInstance::BeginLoadingScreen(const FString& MapName)
{
	if (IsRunningDedicatedServer()) return;

	// MoviePlayer 속성
	FLoadingScreenAttributes Attr;
	Attr.bAutoCompleteWhenLoadingCompletes = true; // 로딩 끝나면 자동 종료
	Attr.MinimumLoadingScreenDisplayTime = 2.f; // 너무 빨리 깜빡임 방지
	Attr.bWaitForManualStop = false; // 수동 종료 여부

	// mainMap 레벨로 전환할 때 특정 로딩 이미지 표시
	Attr.MoviePaths = { TEXT("Loading") };

	GetMoviePlayer()->SetupLoadingScreen(Attr);
	GetMoviePlayer()->PlayMovie();

}

/*void UThirdMotionGameInstance::EndLoadingScreen(UWorld* LoadedWorld)
{
}*/

void UThirdMotionGameInstance::BeginLoadingScreen_WithContext(const FWorldContext& inWorldContext, const FString& MapName)
{
	BeginLoadingScreen(MapName);
}