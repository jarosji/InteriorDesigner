// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "RuntimeMesh/JJRuntimeMesh.h"
#include "Room/JJRoomV2.h"
#include "IDSave.generated.h"

struct IDSaveData
{
	TArray<FRuntimeMeshActorSaveData> RuntimeMeshSaveData;
	TArray<JJRoomSaveData> RoomSaveData;
	
	FORCEINLINE friend  FArchive& operator<<(FArchive &Ar, IDSaveData& SaveGameData ) {
		Ar << SaveGameData.RuntimeMeshSaveData;
		Ar << SaveGameData.RoomSaveData;
		
		return Ar;
	}
};

UCLASS()
class INTERIORDESIGNER_API UIDSave : public UObject
{
	GENERATED_BODY()
public:

	UFUNCTION(BlueprintCallable)
	bool SaveProject(const FString& FullFilePath, const UIDProject* ProjectToSave);

	UFUNCTION(BlueprintCallable)
	bool LoadProject(const FString& FullFilePath, UIDProject* LoadedProject);
    
private:
	void SaveLoadProject(FArchive& Archive, IDSaveData& ProjectData);
};
