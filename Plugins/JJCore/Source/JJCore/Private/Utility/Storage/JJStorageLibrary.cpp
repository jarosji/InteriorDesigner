// Fill out your copyright notice in the Description page of Project Settings.


#include "Utility/Storage/JJStorageLibrary.h"

bool UJJStorageLibrary::CopyFile(const FString& SrcPath, const FString& DstPath)
{
	if(!FPlatformFileManager::Get().GetPlatformFile().DirectoryExists(*FPaths::GetPath(DstPath)))
	{
		FPlatformFileManager::Get().GetPlatformFile().CreateDirectory(*FPaths::GetPath(DstPath));
	}
	if(FPlatformFileManager::Get().GetPlatformFile().FileExists(*SrcPath))
		return FPlatformFileManager::Get().GetPlatformFile().CopyFile(*DstPath, *SrcPath);

	return false;
}
