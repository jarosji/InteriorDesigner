// Fill out your copyright notice in the Description page of Project Settings.

#include "InteriorDesignerSubsystem.h"

#include "IDObjects/IDProject.h"
#include "IDObjects/IDSave.h"
#include "JJAssetSystem/JJAssetSubsystem.h"
#include "Kismet/GameplayStatics.h"

void UInteriorDesignerSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);
}

void UInteriorDesignerSubsystem::Deinitialize()
{
	Super::Deinitialize();
}

void UInteriorDesignerSubsystem::CreateProject(const FString& ProjectName)
{
	UIDProject* NewProject = NewObject<UIDProject>(this);
	NewProject->Init(ProjectName);
	UIDSave* Saver = NewObject<UIDSave>(this);
	Saver->SaveProject(FPaths::SetExtension(FPaths::Combine(FPaths::Combine(FPaths::Combine(FPlatformProcess::UserDir(),
		"InteriorDesigner", "Projects"), ProjectName), ProjectName), ".idproject"),NewProject);

	LoadProject(NewProject->GetProjectPath());
}

void UInteriorDesignerSubsystem::SaveProject(const UIDProject* ProjectToSave)
{
	UIDSave* Saver = NewObject<UIDSave>(this);
	Saver->SaveProject(FPaths::SetExtension(FPaths::Combine(FPaths::Combine(FPaths::Combine(FPlatformProcess::UserDir(),
		"InteriorDesigner", "Projects"), ProjectToSave->GetProjectName()), ProjectToSave->GetProjectName()), ".idproject"), ProjectToSave);
}

UIDProject* UInteriorDesignerSubsystem::LoadProject(const FString& FilePath)
{
	CurrentProject = NewObject<UIDProject>(this);
	CurrentProject->Init(FPaths::GetBaseFilename(FilePath));
	
	GetWorld()->GetSubsystem<UJJAssetSubsystem>()->
		LoadModelAssetsFromFolderAsync(FPaths::Combine(FPaths::GetPath(FilePath), "Models"), true);

	TArray<FString> Extensions;
	Extensions.Add(".png");
	Extensions.Add(".jpg");
	GetWorld()->GetSubsystem<UJJAssetSubsystem>()->
		LoadTexturesFromFolderAsync(FPaths::Combine(FPaths::GetPath(FilePath), "Textures"), Extensions, true);

	UIDSave* Saver = NewObject<UIDSave>(this);
	Saver->LoadProject(FilePath,CurrentProject);

	OnProjectChanged.Broadcast(CurrentProject);
	
	return CurrentProject;
}

void UInteriorDesignerSubsystem::CloseProject()
{
	/*for(AActor* Actor : RuntimeMeshes)
	{
		Actor->Destroy();
	}
	for(AActor* Actor : Rooms)
	{
		Actor->Destroy();
	}
	*/

	UGameplayStatics::OpenLevel(GetWorld(), "InteriorDesigner");
	CurrentProject = nullptr;
	OnProjectChanged.Broadcast(CurrentProject);
}

UIDProject* UInteriorDesignerSubsystem::GetCurrentProject() const
{
	return CurrentProject;
}

void UInteriorDesignerSubsystem::AddRuntimeMeshActor(AJJRuntimeMesh* ActorToAdd)
{
	RuntimeMeshes.Add(ActorToAdd);
}

void UInteriorDesignerSubsystem::AddRoomActor(AJJRoomV2* ActorToAdd)
{
	Rooms.Add(ActorToAdd);
}

TArray<AJJRuntimeMesh*>& UInteriorDesignerSubsystem::GetRuntimeMeshActors()
{
	return RuntimeMeshes;
}

TArray<AJJRoomV2*>& UInteriorDesignerSubsystem::GetRoomActors()
{
	return Rooms;
}
