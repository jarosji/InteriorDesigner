// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class InteriorDesigner : ModuleRules
{
	public InteriorDesigner(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;
	
		PublicDependencyModuleNames.AddRange(new string[] { "Core", "CoreUObject", "Engine","InputCore" });

		PrivateDependencyModuleNames.AddRange(new string[] { "JJAsset", "JJFileImporter", "JJHttp", "Json", "JJGeometryEditor" });

		// Uncomment if you are using Slate UI
		// PrivateDependencyModuleNames.AddRange(new string[] { "Slate", "SlateCore" });
		
		// Uncomment if you are using online features
		// PrivateDependencyModuleNames.Add("OnlineSubsystem");

		// To include OnlineSubsystemSteam, add it to the plugins section in your uproject file with the Enabled attribute set to true
	}
}
