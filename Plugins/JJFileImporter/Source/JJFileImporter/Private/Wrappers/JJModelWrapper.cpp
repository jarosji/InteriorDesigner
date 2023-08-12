// Fill out your copyright notice in the Description page of Project Settings.

#include "Wrappers/JJModelWrapper.h"

void UJJModelWrapper::Init(TSharedPtr<FJJModel> InModel)
{
	WrappedModel = InModel;
}

void UJJModelWrapper::SetImportSettings(const FJJImportSettings& ImportSettings)
{
	WrappedModel->Settings = ImportSettings;
}

TArray<FVector>& UJJModelWrapper::GetPositions(int MeshSection) const
{
	return WrappedModel->Meshes[MeshSection].Geometry.Positions;
}

TArray<int32>& UJJModelWrapper::GetIndices(int MeshSection) const
{
	return WrappedModel->Meshes[MeshSection].Geometry.Indices;
}

TArray<FVector>& UJJModelWrapper::GetNormals(int MeshSection) const
{
	return WrappedModel->Meshes[MeshSection].Geometry.Normals;
}

TArray<FVector2D>& UJJModelWrapper::GetTexCoords(int MeshSection) const
{
	return WrappedModel->Meshes[MeshSection].Geometry.TexCoords;
}

FJJMaterial& UJJModelWrapper::GetJJMaterial(int MeshSection) const
{
	return WrappedModel->Meshes[MeshSection].Material;
}

int UJJModelWrapper::GetNumberOfSections() const
{
	return WrappedModel->Meshes.Num();
}

FString UJJModelWrapper::GetFilePath() const
{
	return WrappedModel->OriginalSrcPath;
}

FString UJJModelWrapper::GetFileName() const
{
	return FPaths::GetBaseFilename(WrappedModel->OriginalSrcPath);
}

TSharedPtr<FJJModel> UJJModelWrapper::GetWrappedModel() const
{
	return WrappedModel;
}
