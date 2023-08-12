#pragma once


struct FJJModel;
class UJJFileImporterSubsystem;

class JJAssetImporter
{
public:
	static bool ImportJJModel(const FString& FullFilePath, TSharedPtr<FJJModel>& ImportedAsset);
	static bool ExportJJModel(const FString& FullFilePath, FJJModel& AssetToSave);

private:
	static void CopyTexture(const FString& SrcFileName, const FString& DestFileName);
	static void SaveLoadJJModel(FArchive& Archive, FJJModel& Asset);
};
