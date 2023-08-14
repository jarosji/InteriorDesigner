// Fill out your copyright notice in the Description page of Project Settings.


#include "IDObjects/IDSave.h"

#include "IDObjects/IDProject.h"
#include "JJAssetSystem/JJAssetSubsystem.h"
#include "InteriorDesignerSubsystem.h"
#include "Room/JJRoomV2.h"
#include "Serialization/BufferArchive.h"

bool UIDSave::SaveProject(const FString& FullFilePath, const UIDProject* ProjectToSave)
{
 	FBufferArchive ToBinary;
	FMemoryWriter DataSavingArchive(ToBinary, true);

	IDSaveData ProjectToSaveData;

	TArray<AJJRuntimeMesh*> RuntimeActors = ProjectToSave->GetWorld()->GetSubsystem<UInteriorDesignerSubsystem>()->RuntimeMeshes;
	for(AJJRuntimeMesh* RuntimeActor : RuntimeActors)
	{
		ProjectToSaveData.RuntimeMeshSaveData.Add(RuntimeActor->CreateSaveData());
	}

	TArray<AJJRoomV2*> RoomActors = ProjectToSave->GetWorld()->GetSubsystem<UInteriorDesignerSubsystem>()->Rooms;
	for(AJJRoomV2* RoomActor : RoomActors)
	{
		ProjectToSaveData.RoomSaveData.Add(RoomActor->CreateSaveData());
	}
	
	SaveLoadProject(DataSavingArchive, ProjectToSaveData);
	
    
	if (ToBinary.Num() <= 0) return false;
	if (FFileHelper::SaveArrayToFile(ToBinary, *FullFilePath))
	{
		ToBinary.FlushCache();
		ToBinary.Empty();
		return true;
	}
    
	ToBinary.FlushCache();
	ToBinary.Empty();
    
	return false;
}

bool UIDSave::LoadProject(const FString& FullFilePath, UIDProject* LoadedProject)
{
	TArray<uint8> TheBinaryArray;
	IDSaveData LoadedData;
	
	if (!FFileHelper::LoadFileToArray(TheBinaryArray, *FullFilePath))
	{
		return false; 
	}   

	if(TheBinaryArray.Num() <= 0) {
		return false;
	}
	
	FMemoryReader FromBinary = FMemoryReader(TheBinaryArray, true);
	FromBinary.Seek(0);
	
	SaveLoadProject(FromBinary, LoadedData);
	
	FromBinary.FlushCache();
	TheBinaryArray.Empty();
	FromBinary.Close();
	
	for(FRuntimeMeshActorSaveData& RuntimeActorSave : LoadedData.RuntimeMeshSaveData)
	{
		FOnRuntimeActorInitializedStatic OnRuntimeActorInitializedStatic;
		OnRuntimeActorInitializedStatic.BindLambda([this, RuntimeActorSave](AJJRuntimeMesh* RuntimeActor){
			RuntimeActor->SetActorTransform(RuntimeActorSave.Transform);

			GetWorld()->GetSubsystem<UInteriorDesignerSubsystem>()
				->AddRuntimeMeshActor(RuntimeActor);
		});
		LoadedProject->GetWorld()->GetSubsystem<UJJAssetSubsystem>()
			->SpawnRuntimeActor(RuntimeActorSave.ModelPath, FVector(), OnRuntimeActorInitializedStatic);
	}
	
	for(JJRoomSaveData& RoomSave : LoadedData.RoomSaveData)
	{
		auto Actor = LoadedProject->GetWorld()->SpawnActor<AJJRoomV2>();
		Actor->BuildMeshDesc(RoomSave);
		Actor->SetActorTransform(RoomSave.Transform);
		Actor->Rebuild();
		
		UTexture2D* Texture;
		if(GetWorld()->GetSubsystem<UJJAssetSubsystem>()->FindTexture(RoomSave.FloorTexturePath, Texture))
			Actor->SetFloorTexture(Texture, RoomSave.FloorTexturePath);

		for(int i=0;i<RoomSave.WallSaves.Num();i++)
		{
			UTexture2D* TextureInner;
			if(GetWorld()->GetSubsystem<UJJAssetSubsystem>()->FindTexture(RoomSave.WallSaves[i].InnerTexturePath, TextureInner))
				Actor->GetWalls()[i]->SetTexture(ERoomMaterialType::WallInside, Texture, RoomSave.WallSaves[i].InnerTexturePath);

			UTexture2D* TextureOuter;
			if(GetWorld()->GetSubsystem<UJJAssetSubsystem>()->FindTexture(RoomSave.WallSaves[i].OuterTexturePath, TextureOuter))
				Actor->GetWalls()[i]->SetTexture(ERoomMaterialType::WallOutside, TextureOuter, RoomSave.WallSaves[i].OuterTexturePath);
		}
		
		Actor->SetClosedLoop(RoomSave.bClosedLoop);

		GetWorld()->GetSubsystem<UInteriorDesignerSubsystem>()->AddRoomActor(Actor);
	}
	
	return true;
}

void UIDSave::SaveLoadProject(FArchive& Archive, IDSaveData& ProjectData)
{
	Archive << ProjectData;
}


