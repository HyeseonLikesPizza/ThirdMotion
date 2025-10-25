// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class ThirdMotion : ModuleRules
{
	public ThirdMotion(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

		PublicDependencyModuleNames.AddRange(new string[] { "Core", "CoreUObject", "Engine", "InputCore", "EnhancedInput", "UMG", "MoviePlayer", "Sockets", "Networking", "GameplayTags", "NetCore" });

		PrivateDependencyModuleNames.AddRange(new string[] { "Slate", "SlateCore", "ImageWrapper", "MoviePlayer", "MoviePlayer", "UMG" });

		// 에디터 전용 모듈 추가
		if (Target.bBuildEditor)
		{
			PrivateDependencyModuleNames.Add("UnrealEd");
		}

		PublicIncludePaths.Add(ModuleDirectory + "/Public");
		PrivateIncludePaths.Add(ModuleDirectory + "/Private");

		// Uncomment if you are using online features
		// PrivateDependencyModuleNames.Add("OnlineSubsystem");

		// To include OnlineSubsystemSteam, add it to the plugins section in your uproject file with the Enabled attribute set to true
	}
}
