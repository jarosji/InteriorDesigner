// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class JJFileImporter : ModuleRules
{
	public JJFileImporter(ReadOnlyTargetRules Target) : base(Target)
	{
		PublicDependencyModuleNames.AddRange(
			new string[]
			{
				"Core", "Engine",
				// ... add other public dependencies that you statically link with here ...
			}
			);
			
		
		PrivateDependencyModuleNames.AddRange(
			new string[]
			{
				"Projects",
				"CoreUObject",
				"MeshDescription", "StaticMeshDescription", "MeshConversion",
				"AssimpLibrary",
				"RenderCore",
				"RHI"
				// ... add private dependencies that you statically link with here ...	
			}
			);
	}
}
