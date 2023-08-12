@echo off
echo !!! Ensure that I'm located next to a desired .uplugin file !!!
echo !! After creation - You have to edit the .uplugin file (Not yet automatic) !!
echo ! In Rider - Click on "RefreshSolution" to see the newly created module in Solution Explorer !
set /p ModuleName=Name of the Module: 

md Source\ThirdParty\%ModuleName%

(
echo // Fill out your copyright notice in the Description page of Project Settings.
echo:
echo using System.IO;
echo using UnrealBuildTool;
echo:
echo public class %ModuleName% : ModuleRules
echo {
echo	public %ModuleName%(ReadOnlyTargetRules Target^) : base(Target^)
echo	{
echo		Type = ModuleType.External;
echo:
echo		if (Target.Platform == UnrealTargetPlatform.Win64^)
echo		{
echo			// Add any include paths for the plugin
echo			PublicIncludePaths.Add(Path.Combine(ModuleDirectory, "Path/To/Include", "include"^)^);
echo:			
echo			// Add the import library
echo			PublicAdditionalLibraries.Add(Path.Combine(ModuleDirectory, "Path/To/Lib", "example.lib"^)^);
echo:
echo			// Delay-load the DLL, so we can load it from the right place first
echo			PublicDelayLoadDLLs.Add("example.dll"^);
echo:
echo			// Ensure that the DLL is staged along with the executable
echo			RuntimeDependencies.Add(Path.Combine(ModuleDirectory, "Path/To/.dll", "example.dll"^)^);
echo        }
echo	}
echo }
)> Source\ThirdParty\%ModuleName%\%ModuleName%.build.cs"

