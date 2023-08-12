#include "Utility/JJModelConverter.h"

#include "MeshDescriptionBuilder.h"
#include "StaticMeshAttributes.h"
#include "ImportedFiles/JJModel.h"

FMeshDescription JJModelConverter::ConvertJJModelToMeshDescription(const FJJModel& ModelToConvert)
{
	FMeshDescription meshDesc;
	FStaticMeshAttributes Attributes(meshDesc);
	Attributes.Register();
			
	FMeshDescriptionBuilder meshDescBuilder;
	meshDescBuilder.SetMeshDescription(&meshDesc);
	meshDescBuilder.EnablePolyGroups();
	meshDescBuilder.SetNumUVLayers(1);

	TArray< FVertexID > vertexIDs;
	TArray< FUVID > UVIDs;
			
	int offset = 0;
	for(const FJJMesh& Mesh : ModelToConvert.Meshes)
	{
		for(int i =0;i<Mesh.Geometry.Positions.Num();i++)
		{
			const FVector& Vector = Mesh.Geometry.Positions[i];
			const FVector2D& TexCoord = Mesh.Geometry.TexCoords[i];
			
			vertexIDs.Add(meshDescBuilder.AppendVertex(Vector));
			UVIDs.Add(meshDescBuilder.AppendUV(TexCoord, 0));
		}
				
		FPolygonGroupID polygonGroup = meshDescBuilder.AppendPolygonGroup();
		for(int i =0; i<Mesh.Geometry.Indices.Num(); i+=3)
		{
			int32 Index0 = Mesh.Geometry.Indices[i];
			int32 Index1 = Mesh.Geometry.Indices[i + 1];
			int32 Index2 = Mesh.Geometry.Indices[i + 2];
					
			FVertexInstanceID instance0 = meshDescBuilder.AppendInstance(vertexIDs[Index0 + offset]);
			meshDescBuilder.SetInstanceNormal(instance0, Mesh.Geometry.Normals[Index0]);
			meshDescBuilder.SetInstanceColor(instance0, FVector4f(1.0f, 1.0f, 1.0f, 1.0f));

			FVertexInstanceID instance1 = meshDescBuilder.AppendInstance(vertexIDs[Index1 + offset]);
			meshDescBuilder.SetInstanceNormal(instance1, Mesh.Geometry.Normals[Index1]);
			meshDescBuilder.SetInstanceColor(instance1, FVector4f(1.0f, 1.0f, 1.0f, 1.0f));

			FVertexInstanceID instance2 = meshDescBuilder.AppendInstance(vertexIDs[Index2 + offset]);
			meshDescBuilder.SetInstanceNormal(instance2, Mesh.Geometry.Normals[Index2]);
			meshDescBuilder.SetInstanceColor(instance2, FVector4f(1.0f, 1.0f, 1.0f, 1.0f));

			FTriangleID TriID = meshDescBuilder.AppendTriangle(instance0, instance1, instance2, polygonGroup);
			meshDescBuilder.AppendUVTriangle(TriID, UVIDs[Index0+offset], UVIDs[Index1+offset], UVIDs[Index2+offset], 0);
		}
		offset += Mesh.Geometry.Positions.Num();
	}

	return meshDesc;
}
