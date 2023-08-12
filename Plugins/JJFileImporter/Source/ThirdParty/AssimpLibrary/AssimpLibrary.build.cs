// Fill out your copyright notice in the Description page of Project Settings.

using System.IO;
using UnrealBuildTool;

public class AssimpLibrary : ModuleRules
{
	public AssimpLibrary(ReadOnlyTargetRules Target) : base(Target)
	{
		Type = ModuleType.External;

		if (Target.Platform == UnrealTargetPlatform.Win64)
		{
			// Add any include paths for the plugin
			PublicIncludePaths.Add(Path.Combine(ModuleDirectory, "assimp", "include"));
				
			// Add the import library
			PublicAdditionalLibraries.Add(Path.Combine(ModuleDirectory,
				"assimp", "lib", "Release", "assimp-vc142-mt.lib"));

			// Delay-load the DLL, so we can load it from the right place first
			PublicDelayLoadDLLs.Add("assimp-vc142-mt.dll");

			// Ensure that the DLL is staged along with the executable
			RuntimeDependencies.Add(Path.Combine(ModuleDirectory,
				"assimp", "bin", "Release", "assimp-vc142-mt.dll"));
		}
	}
}
