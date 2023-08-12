#pragma once

template<typename TReal>
class aiMatrix4x4t;
typedef float ai_real;
typedef aiMatrix4x4t<ai_real> aiMatrix4x4;

struct aiScene;
struct aiNode;
struct aiMesh;
struct aiMaterial;

struct FJJModel;
struct FJJMesh;
struct FJJGeometry;
struct FJJMaterial;

class JJFILEIMPORTER_API JJModelImporter
{
public:
	JJModelImporter(const FString& FilePath);
	
	TSharedRef<FJJModel> ImportModel();

private:
	FString FilePath;

	void ProcessNode(const aiNode& AiNode, const aiScene* AiScene, FJJModel& ImportedModel);	
	void ProcessGeometry(const aiMesh& AiMesh,  const aiMatrix4x4& MeshTransform, FJJGeometry& MeshGeometry);
	void ProcessMaterial(const aiMaterial& AiMaterial, FJJMaterial& MeshMaterial);
};