#include "JJModelImporter.h"

#include "assimp/scene.h"
#include "assimp/Importer.hpp"
#include "assimp/postprocess.h"

#include "ImportedFiles/JJModel.h"
#include "Importers/TextureImporter/JJTextureImporter.h"

JJModelImporter::JJModelImporter(const FString& FilePath) : FilePath(FilePath)
{
	
}

TSharedRef<FJJModel> JJModelImporter::ImportModel()
{
	TSharedRef<FJJModel> ImportedModel = MakeShared<FJJModel>();
	ImportedModel->OriginalSrcPath = FPaths::GetBaseFilename(FilePath);
	
	Assimp::Importer Importer;
	Importer.SetPropertyInteger(AI_CONFIG_PP_SBP_REMOVE, aiPrimitiveType_POINT | aiPrimitiveType_LINE);
	const aiScene* AiScene = Importer.ReadFile(TCHAR_TO_ANSI(*FilePath), aiProcess_Triangulate | aiProcess_SortByPType |
		aiProcess_GenSmoothNormals | aiProcess_JoinIdenticalVertices | aiProcess_FlipUVs);
	
	if(FPaths::GetExtension(FilePath) == "obj")
		ImportedModel->Settings.Transform.SetScale3D(FVector(100));
	ImportedModel->Meshes.SetNum(AiScene->mNumMeshes);

	/*float Factor(0.0);
	if(AiScene->mMetaData->Get("UnitScaleFactor", Factor))
	{
		aiMatrix4x4 Mat, ScaleMat;
		Mat.Scaling(aiVector3D(Factor/100),ScaleMat);
		AiScene->mRootNode->mTransformation = AiScene->mRootNode->mTransformation * (Factor/100);
	}*/
	
	ProcessNode(*AiScene->mRootNode, AiScene, ImportedModel.Get());

	return ImportedModel;
}

void JJModelImporter::ProcessNode(const aiNode& AiNode, const aiScene* AiScene, FJJModel& ImportedModel)
{
	for(unsigned int i = 0; i < AiNode.mNumMeshes; i++)
	{
		aiMesh* AiMesh = AiScene->mMeshes[AiNode.mMeshes[i]];
		FJJMesh& ImportedMesh = ImportedModel.Meshes[AiNode.mMeshes[i]];
		ImportedMesh.Material.MaterialIndex = AiMesh->mMaterialIndex;
		
		ProcessGeometry(*AiMesh, AiNode.mTransformation, ImportedMesh.Geometry);
		ProcessMaterial(*AiScene->mMaterials[AiMesh->mMaterialIndex], ImportedMesh.Material);
	}

	for(unsigned int i = 0; i < AiNode.mNumChildren; i++)
	{
		AiNode.mChildren[i]->mTransformation *= AiNode.mTransformation;
		ProcessNode(*AiNode.mChildren[i], AiScene, ImportedModel);
	}
}

void JJModelImporter::ProcessGeometry(const aiMesh& AiMesh, const aiMatrix4x4& MeshTransform, FJJGeometry& MeshGeometry)
{
	MeshGeometry.NumIndices = AiMesh.mNumFaces * 3;
	MeshGeometry.NumVertices = AiMesh.mNumVertices;
	
	MeshGeometry.Positions.Reserve(MeshGeometry.NumVertices);
	MeshGeometry.Indices.Reserve(MeshGeometry.NumIndices);
	MeshGeometry.Normals.Reserve(MeshGeometry.NumVertices);
	MeshGeometry.TexCoords.Reserve(MeshGeometry.NumVertices);
	
	for(uint32 i = 0; i < AiMesh.mNumVertices; i++)
	{
		const aiVector3D& Vertex = MeshTransform * AiMesh.mVertices[i];
		const aiVector3D& Normal = AiMesh.mNormals[i];
		const aiVector3D& TexCoord = AiMesh.HasTextureCoords(0) ? AiMesh.mTextureCoords[0][i] : aiVector3D(0);
		
		MeshGeometry.Positions.Push(FVector(Vertex.x, Vertex.z, Vertex.y));
		MeshGeometry.Normals.Push(FVector(Normal.x, Normal.z, Normal.y));
		MeshGeometry.TexCoords.Push(FVector2D(TexCoord.x, TexCoord.y));

		FVector Test(Vertex.x, Vertex.z, Vertex.y);
	}
	
	for(uint32 i = 0; i < AiMesh.mNumFaces; i++)
	{
		const aiFace& Face = AiMesh.mFaces[i];
		if(Face.mNumIndices == 3){
			MeshGeometry.Indices.Push(Face.mIndices[0]);
			MeshGeometry.Indices.Push(Face.mIndices[1]);
			MeshGeometry.Indices.Push(Face.mIndices[2]);
		}
	}
}

void JJModelImporter::ProcessMaterial(const aiMaterial& AiMaterial, FJJMaterial& MeshMaterial)
{
	//Diffuse
	{
		aiColor3D DiffuseColor;
		if(AiMaterial.Get(AI_MATKEY_COLOR_DIFFUSE, DiffuseColor)  == AI_SUCCESS)
		{
			MeshMaterial.DiffuseColor = FVector(DiffuseColor.r, DiffuseColor.g, DiffuseColor.b);
		}
	}

	//Opacity
	{
		ai_real Opacity;
		if(AiMaterial.Get(AI_MATKEY_OPACITY, Opacity)  == AI_SUCCESS)
		{
			MeshMaterial.Opacity = Opacity;
		}
	}

	//Material
	if(AiMaterial.GetTextureCount(aiTextureType_DIFFUSE) > 0)
	{
		aiString Path;
		if(AiMaterial.GetTexture(aiTextureType_DIFFUSE, 0, &Path, nullptr, nullptr,nullptr,nullptr,nullptr) == AI_SUCCESS)
		{
			const FString& FullPath = FPaths::Combine(FPaths::GetPath(FilePath), Path.data);
			MeshMaterial.SrcPath = FullPath;
			MeshMaterial.RelativePath = Path.data;
			
			//Material.Texture = FImageUtils::ImportFileAsTexture2D(FullPath);//NewObject<UTexture2D>();
			//TextureImporter::LoadTextureAsync(FullPath, Material.Texture);

			MeshMaterial.Texture = NewObject<UTexture2D>();
			JJTextureImporter LoadTextureAsync;
			LoadTextureAsync.ImportTexture(FullPath, MeshMaterial.Texture);
		}
	}
}
