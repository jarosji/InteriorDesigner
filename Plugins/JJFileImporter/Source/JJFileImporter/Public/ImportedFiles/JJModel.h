#pragma once

#include "JJModel.generated.h"

USTRUCT(BlueprintType)
struct JJFILEIMPORTER_API FJJImportSettings
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadWrite)
	FTransform Transform;

	UPROPERTY(BlueprintReadWrite)
	bool bDoesEditArchitecture;

	FORCEINLINE friend  FArchive& operator<<(FArchive &Ar, FJJImportSettings& SaveGameData ) {
		Ar << SaveGameData.Transform;
		
		return Ar;
	}
};

struct JJFILEIMPORTER_API FJJGeometry
{	
	TArray<FVector> Positions;
	TArray<int32> Indices;
	TArray<FVector> Normals;
	TArray<FVector2D> TexCoords;

	uint32 NumIndices;
	uint32 NumVertices;

	FORCEINLINE friend  FArchive& operator<<(FArchive &Ar, FJJGeometry& SaveGameData ) {

		Ar << SaveGameData.Positions;
		Ar << SaveGameData.Indices;
		Ar << SaveGameData.Normals;
		Ar << SaveGameData.TexCoords;

		Ar << SaveGameData.NumIndices;
		Ar << SaveGameData.NumVertices;
		
		return Ar;
	}
};

USTRUCT(BlueprintType)
struct JJFILEIMPORTER_API FJJMaterial
{
	GENERATED_BODY()
	
	UPROPERTY(BlueprintReadOnly)
	FString SrcPath;

	UPROPERTY(BlueprintReadOnly)
	FString DestPath;

	UPROPERTY(BlueprintReadOnly)
	FString RelativePath;

	UPROPERTY(BlueprintReadOnly)
	int32 MaterialIndex;

	UPROPERTY(BlueprintReadOnly)
	FVector DiffuseColor;

	UPROPERTY(BlueprintReadOnly)
	float Opacity;

	UPROPERTY(BlueprintReadOnly)
	UTexture2D* Texture = nullptr;

	FORCEINLINE friend  FArchive& operator<<(FArchive &Ar, FJJMaterial& SaveGameData ) {
		
		Ar << SaveGameData.SrcPath;
		Ar << SaveGameData.DestPath;
		Ar << SaveGameData.RelativePath;
		
		Ar << SaveGameData.MaterialIndex;
		Ar << SaveGameData.DiffuseColor;
		Ar << SaveGameData.Opacity;
		
		return Ar;
	}
};

struct JJFILEIMPORTER_API FJJMesh
{	
	FJJGeometry Geometry;

	FJJMaterial Material;
	
	FORCEINLINE friend FArchive& operator<<(FArchive &Ar, FJJMesh& SaveGameData ) {
		Ar << SaveGameData.Geometry;
		Ar << SaveGameData.Material;
			
		return Ar;
	}
};

struct JJFILEIMPORTER_API FJJModel
{
	FJJImportSettings Settings;
	
	FString OriginalSrcPath;

	uint32 TotalIndices;
	uint32 TotalVertices;
	
	TArray<FJJMesh> Meshes;

	FORCEINLINE friend  FArchive& operator<<(FArchive &Ar, FJJModel& SaveGameData ) {
		Ar << SaveGameData.OriginalSrcPath;
		//Ar << SaveGameData.DestPath;
		Ar << SaveGameData.Settings;

		Ar << SaveGameData.TotalIndices;
		Ar << SaveGameData.TotalVertices;

		Ar << SaveGameData.Meshes;
		
		return Ar;
	}
};
