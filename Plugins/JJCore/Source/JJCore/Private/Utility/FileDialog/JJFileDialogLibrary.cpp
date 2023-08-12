// Fill out your copyright notice in the Description page of Project Settings.


#include "Utility/FileDialog/JJFileDialogLibrary.h"

#include "Windows/AllowWindowsPlatformTypes.h"
#include <Windows/MinWindows.h>
#include <commdlg.h>
#include "Windows/HideWindowsPlatformTypes.h"

bool UJJFileDialogLibrary::OpenFileDialog(FString& FilePath, const FString& DialogTitle, const FString& DefaultPath,
		const TArray<FString>& Extensions, const TArray<FString>& Descriptions)
{
	OPENFILENAMEA File;
	CHAR szFile[MAX_PATH_SIZE] = { 0 };
	ZeroMemory(&File, sizeof(OPENFILENAMEA)); 
	
	const std::string& Filter = FormatFilter(Extensions, Descriptions);
	
	File.lStructSize = sizeof(OPENFILENAMEA);
	File.hwndOwner = static_cast<HWND>(GEngine->GameViewport->GetWindow()->GetNativeWindow()->GetOSWindowHandle());
	File.lpstrFile = szFile;
	File.nMaxFile = sizeof(szFile);
	File.lpstrFilter = Filter.c_str();
	File.lpstrTitle = TCHAR_TO_ANSI(*DialogTitle);
	File.lpstrInitialDir = TCHAR_TO_ANSI(*DefaultPath);
	File.nFilterIndex = 1;
	File.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST | OFN_NOCHANGEDIR;

	if(GetOpenFileNameA(&File))
	{
		FilePath = File.lpstrFile;
		return true;
	}
	
	return false;
}

std::string UJJFileDialogLibrary::FormatFilter(const TArray<FString>& Extensions, const TArray<FString>& Descriptions)
{
	std::string Filter = "";
	
	for(int i = 0; i < Extensions.Num(); i++)
	{
		const FString& Description = Descriptions.IsValidIndex(i) ? Descriptions[i] : Extensions[i];		
		Filter += std::string(TCHAR_TO_UTF8(*Description));
		Filter.push_back('\0');
		
		TArray<FString> SplitExtensions;
		Extensions[i].ParseIntoArray(SplitExtensions, TEXT("|"));
		for(int j = 0; j < SplitExtensions.Num(); j++)
		{
			SplitExtensions[j].RemoveSpacesInline();
			const FString& FinalExtension = SplitExtensions[j].Contains(".") ? SplitExtensions[j] : "." + SplitExtensions[j];
			Filter += "*" + std::string(TCHAR_TO_UTF8(*FinalExtension));
			if(j<SplitExtensions.Num()-1) Filter += ";";
		}
		Filter.push_back('\0');
	}

	return Filter;
}
