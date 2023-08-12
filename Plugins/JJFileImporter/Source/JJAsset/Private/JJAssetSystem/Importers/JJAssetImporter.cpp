#include "JJAssetSystem/Importers/JJAssetImporter.h"

#include "ImportedFiles/JJModel.h"
#include "Serialization/BufferArchive.h"

bool JJAssetImporter::ImportJJModel(const FString& FullFilePath, TSharedPtr<FJJModel>& ImportedAsset)
{
	TArray<uint8> TheBinaryArray;
	ImportedAsset = MakeShared<FJJModel>();
	
	if (!FFileHelper::LoadFileToArray(TheBinaryArray, *FullFilePath))
	{
		return false; 
	}   

	if(TheBinaryArray.Num() <= 0) {
		return false;
	}
	
	FMemoryReader FromBinary = FMemoryReader(TheBinaryArray, true);
	FromBinary.Seek(0);
	
	SaveLoadJJModel(FromBinary, *ImportedAsset.Get());
	
	FromBinary.FlushCache();
	TheBinaryArray.Empty(); FromBinary.Close();

	ImportedAsset->OriginalSrcPath = FullFilePath;

	for(FJJMesh& Mesh : ImportedAsset->Meshes)
	{
		Mesh.Material.SrcPath = Mesh.Material.DestPath;
	}

	return true;
}

bool JJAssetImporter::ExportJJModel(const FString& FullFilePath, FJJModel& AssetToSave)
{
	FBufferArchive ToBinary;
	FMemoryWriter DataSavingArchive(ToBinary, true);

	//Copy textures
	for(FJJMesh& Mesh : AssetToSave.Meshes)
	{
		//TextureImporter::SaveTexture(Mesh.Material.SrcPath, FPaths::Combine(FPaths::ProjectContentDir(), "Models", Mesh.Material.RelativePath));
		Mesh.Material.DestPath = FPaths::Combine(FPaths::GetPath(FullFilePath), Mesh.Material.RelativePath);
		CopyTexture(Mesh.Material.SrcPath, Mesh.Material.DestPath);
	}
	
	SaveLoadJJModel(DataSavingArchive, AssetToSave);

	if (ToBinary.Num() <= 0) return false;
	if (FFileHelper::SaveArrayToFile(ToBinary, *FullFilePath))
	{
		ToBinary.FlushCache();
		ToBinary.Empty();
	}

	ToBinary.FlushCache();
	ToBinary.Empty();

	return true;
}

void JJAssetImporter::CopyTexture(const FString& SrcFileName, const FString& DestFileName)
{
	if(!FPlatformFileManager::Get().GetPlatformFile().DirectoryExists(*FPaths::GetPath(DestFileName)))
	{
		FPlatformFileManager::Get().GetPlatformFile().CreateDirectory(*FPaths::GetPath(DestFileName));
	}
	if(FPlatformFileManager::Get().GetPlatformFile().FileExists(*SrcFileName))
		FPlatformFileManager::Get().GetPlatformFile().CopyFile(*DestFileName, *SrcFileName);
}

void JJAssetImporter::SaveLoadJJModel(FArchive& Archive, FJJModel& Asset)
{
	Archive << Asset;
}