@echo off
echo !!! Ensure that I'm located next to a desired .uplugin file !!!
echo !! After creation - You have to edit the .uplugin file (Not yet automatic) !!
echo ! In Rider - Click on "RefreshSolution" to see the newly created module in Solution Explorer !
set /p ModuleName=Name of the Module: 

set FullModuleName=F%ModuleName%Module

md Source\%ModuleName%
md Source\%ModuleName%\Private
md Source\%ModuleName%\Public

(
echo // Copyright Epic Games, Inc. All Rights Reserved.
echo: 
echo using UnrealBuildTool;
echo: 
echo public class %ModuleName% : ModuleRules
echo {
echo 	public %ModuleName%(ReadOnlyTargetRules Target^) : base(Target^)
echo 	{
echo 		PCHUsage = ModuleRules.PCHUsageMode.UseExplicitOrSharedPCHs;
echo: 		
echo 		PublicIncludePaths.AddRange(
echo 			new string[] {
echo 				// ... add public include paths required here ...
echo 			}
echo 			^);
echo: 				
echo: 		
echo 		PrivateIncludePaths.AddRange(
echo 			new string[] {
echo 				// ... add other private include paths required here ...
echo 			}
echo 			^);
echo: 			
echo: 		
echo 		PublicDependencyModuleNames.AddRange(
echo 			new string[]
echo 			{
echo 				"Core",
echo 				// ... add other public dependencies that you statically link with here ...
echo 			}
echo 			^);
echo: 			
echo: 		
echo 		PrivateDependencyModuleNames.AddRange(
echo 			new string[]
echo 			{
echo 				"CoreUObject",
echo 				"Engine",
echo 				// ... add private dependencies that you statically link with here ...	
echo 			}
echo 			^);
echo: 		
echo: 		
echo 		DynamicallyLoadedModuleNames.AddRange(
echo 			new string[]
echo 			{
echo 				// ... add any modules that your module loads dynamically here ...
echo 			}
echo 			^);
echo 	}
echo }
)> Source\%ModuleName%\%ModuleName%.build.cs"

(
echo // Copyright Epic Games, Inc. All Rights Reserved.
echo:
echo #pragma once
echo:
echo #include "CoreMinimal.h"
echo #include "Modules/ModuleManager.h"
echo:
echo class %FullModuleName% : public IModuleInterface
echo {
echo public:
echo:
echo	/** IModuleInterface implementation */
echo	virtual void StartupModule(^) override;
echo	virtual void ShutdownModule(^) override;
echo };
)> Source\%ModuleName%\Public\%ModuleName%.h"

(
echo // Copyright Epic Games, Inc. All Rights Reserved.
echo:
echo #include "%ModuleName%.h"
echo:
echo #define LOCTEXT_NAMESPACE "%FullModuleName%"
echo:
echo void %FullModuleName%::StartupModule(^)
echo {
echo	// This code will execute after your module is loaded into memory; the exact timing is specified in the .uplugin file per-module
echo }
echo:
echo void %FullModuleName%::ShutdownModule(^)
echo {
echo	// This function may be called during shutdown to clean up your module.  For modules that support dynamic reloading,
echo	// we call this function before unloading the module.
echo }
echo:
echo #undef LOCTEXT_NAMESPACE
echo:	
echo IMPLEMENT_MODULE(%FullModuleName%, %ModuleName%^)
)> Source\%ModuleName%\Private\%ModuleName%.cpp"

