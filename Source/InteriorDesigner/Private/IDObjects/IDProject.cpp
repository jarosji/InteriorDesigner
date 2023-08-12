// Fill out your copyright notice in the Description page of Project Settings.


#include "IDObjects/IDProject.h"

void UIDProject::Init(const FString& Name)
{
	ProjectName = Name;
	ProjectDir = FPaths::Combine(FPlatformProcess::UserDir(), "InteriorDesigner", "Projects", ProjectName);
}

const FString& UIDProject::GetProjectName() const
{
	return ProjectName;
}

const FString& UIDProject::GetProjectDir() const
{
	return ProjectDir;
}

FString UIDProject::GetProjectModelDir() const
{
	return FPaths::Combine(ProjectDir, "Models");
}

FString UIDProject::GetProjectTextureDir() const
{
	return FPaths::Combine(ProjectDir, "Textures");
}

FString UIDProject::GetProjectPath() const
{
	return FPaths::SetExtension(FPaths::Combine(ProjectDir, ProjectName), ".idproject");
}
