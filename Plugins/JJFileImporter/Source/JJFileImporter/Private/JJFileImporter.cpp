// Copyright Epic Games, Inc. All Rights Reserved.

#include "JJFileImporter.h"

#include "Core.h"
#include "Interfaces/IPluginManager.h"
#include "Modules/ModuleManager.h"

#define LOCTEXT_NAMESPACE "FJJFileImporterModule"

void FJJFileImporterModule::StartupModule()
{
	// This code will execute after your module is loaded into memory; the exact timing is specified in the .uplugin file per-module
    
    	// Get the base directory of this plugin
    	FString BaseDir = IPluginManager::Get().FindPlugin("JJFileImporter")->GetBaseDir();
    
    	// Add on the relative location of the third party dll and load it
    	FString LibraryPath = FPaths::Combine(*BaseDir, TEXT("Source/ThirdParty/AssimpLibrary/assimp/bin/Release/assimp-vc142-mt.dll"));
    	
    	ExampleLibraryHandle = !LibraryPath.IsEmpty() ? FPlatformProcess::GetDllHandle(*LibraryPath) : nullptr;
    
    	if (ExampleLibraryHandle)
    	{
    		//Succesfuly loaded
    	}
    	else
    	{
    		FMessageDialog::Open(EAppMsgType::Ok, LOCTEXT("ThirdPartyLibraryError", "Failed to load example third party library"));
    	}
}

void FJJFileImporterModule::ShutdownModule()
{
	// This function may be called during shutdown to clean up your module.  For modules that support dynamic reloading,
	// we call this function before unloading the module.

	// Free the dll handle
	FPlatformProcess::FreeDllHandle(ExampleLibraryHandle);
	ExampleLibraryHandle = nullptr;
}

#undef LOCTEXT_NAMESPACE
	
IMPLEMENT_MODULE(FJJFileImporterModule, JJFileImporter)