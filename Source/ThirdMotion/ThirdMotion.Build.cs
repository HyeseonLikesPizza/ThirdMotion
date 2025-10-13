// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class ThirdMotion : ModuleRules
{
	public ThirdMotion(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;
	
		PublicDependencyModuleNames.AddRange(new string[] { "Core", "CoreUObject", "Engine", "InputCore", "EnhancedInput", "UMG", "Sockets", "Networking", "GameplayTags", "NetCore" });

		PrivateDependencyModuleNames.AddRange(new string[] { "Slate", "SlateCore", "ImageWrapper" });
		
		PublicIncludePaths.Add(ModuleDirectory + "/Public");
		PrivateIncludePaths.Add(ModuleDirectory + "/Private");
		
		// Uncomment if you are using online features
		// PrivateDependencyModuleNames.Add("OnlineSubsystem");

		// To include OnlineSubsystemSteam, add it to the plugins section in your uproject file with the Enabled attribute set to true
	}
}
