// Fill out your copyright notice in the Description page of Project Settings.

#include "RuntimeMesh/JJRuntimeMesh.h"

#include "JJFileImporterSubsystem.h"
#include "MeshDescription.h"
#include "ImportedFiles/JJModel.h"
#include "JJAssetSystem/JJAssetSubsystem.h"
#include "Utility/JJModelConverter.h"

void UCollisionProvider::Init(AJJRuntimeMesh* InRuntimeMeshActor)
{
	OwningActor = InRuntimeMeshActor;
}

bool UCollisionProvider::GetPhysicsTriMeshData(FTriMeshCollisionData* CollisionData, bool InUseAllTriData)
{
	{
		int Offset = 0;
		for(FJJMesh Mesh : OwningActor->Model->Meshes)
		{
			for(int i = 0; i < Mesh.Geometry.Positions.Num();i++)
			{
				FVector3f Vec = FVector3f(
					Mesh.Geometry.Positions[i].X,
					Mesh.Geometry.Positions[i].Y,
					Mesh.Geometry.Positions[i].Z);
				CollisionData->Vertices.Add(Vec);
			}
			if (CollisionData->UVs.Num() < 1)
			{
				CollisionData->UVs.SetNum(1);
			}
			CollisionData->UVs[0].AddZeroed(Mesh.Geometry.Positions.Num());
				
			for(int i = 0; i < Mesh.Geometry.Indices.Num();i+=3)
			{
				FTriIndices& Tri = CollisionData->Indices.AddDefaulted_GetRef();
				Tri.v0 = Mesh.Geometry.Indices[i + 0] + Offset;
				Tri.v1 = Mesh.Geometry.Indices[i + 1] + Offset;
				Tri.v2 = Mesh.Geometry.Indices[i + 2] + Offset;

				//CollisionData->MaterialIndices.Add(Config.MaterialSlot);
			}
			Offset += Mesh.Geometry.NumVertices;
		}
		return true;
	}
}

bool UCollisionProvider::ContainsPhysicsTriMeshData(bool InUseAllTriData) const
{
	return OwningActor->Model.IsValid();
}

bool UCollisionProvider::WantsNegXTriMesh()
{
	return false;
}

void UCollisionProvider::GetMeshId(FString& OutMeshId)
{
	OutMeshId = GetName();
}

AJJRuntimeMesh::AJJRuntimeMesh()
{
	PrimaryActorTick.bCanEverTick = false;

	StaticMeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("StaticMeshComponent"), false);
	SetRootComponent(StaticMeshComponent);

}

void AJJRuntimeMesh::Init(TSharedPtr<FJJModel> InModel, bool bInIsCPUAccessible, bool bInSaveModelInMemory)
{
	Model = InModel;
	bIsCPUAccessible = bInIsCPUAccessible;
	bSaveModelInMemory = bInSaveModelInMemory;
}

FRuntimeMeshActorSaveData AJJRuntimeMesh::CreateSaveData() const
{
	FRuntimeMeshActorSaveData Save;
	Save.Transform = GetTransform();
	//Save.ModelPath = FPaths::Combine(FPaths::Combine(FPaths::ProjectContentDir(), "Models"), Model->PathInfo.Name + ".jjasset");
	Save.ModelPath = Model->OriginalSrcPath;

	return Save;
}

void AJJRuntimeMesh::BeginPlay()
{
	Super::BeginPlay();

	UJJAssetSubsystem* AssetSubsystem = GetWorld()->GetSubsystem<UJJAssetSubsystem>();
	DynamicMaterialDiffuse = AssetSubsystem->DynamicMaterialDiffuse;
	DynamicMaterialOpacity = AssetSubsystem->DynamicMaterialOpacity;
}

void AJJRuntimeMesh::RebuildModel()
{
	StaticMesh = NewObject<UStaticMesh>();
	StaticMeshComponent->SetStaticMesh(StaticMesh);

	//Unreal will try to update UVs, but that functionality is only accessible in Editor -> Crahes in packaged builds.
	FStaticMaterial StatMat0;
	StatMat0.UVChannelData.bInitialized = true;
	FStaticMaterial StatMat1;
	StatMat1.UVChannelData.bInitialized = true;
	StaticMesh->GetStaticMaterials().Add(StatMat0);
	StaticMesh->GetStaticMaterials().Add(StatMat1);

	AsyncTask(ENamedThreads::AnyBackgroundHiPriTask, [this]
	{
		const FMeshDescription MeshDesc = JJModelConverter::ConvertJJModelToMeshDescription(*Model.Get());
		TArray<const FMeshDescription*> meshDescPtrs;
		meshDescPtrs.Emplace(&MeshDesc);
		
		UStaticMesh::FBuildMeshDescriptionsParams mdParams;
		mdParams.bBuildSimpleCollision = false;
		mdParams.bFastBuild = true;
		mdParams.bCommitMeshDescription = bIsCPUAccessible;
		mdParams.bMarkPackageDirty = false;
		
		StaticMesh->BuildFromMeshDescriptions(meshDescPtrs, mdParams);
		
		AsyncTask(ENamedThreads::GameThread, [this]
		{
			FStaticMeshRenderData* const RenderData = StaticMesh->GetRenderData();

			int32 LODIndex = 0;
			int32 MaxLODs = RenderData->LODResources.Num();
			int32 MaterialID = 0;
			for (; LODIndex < MaxLODs; ++LODIndex)
			{
				FStaticMeshLODResources& LOD = RenderData->LODResources[LODIndex];

				for (int32 SectionIndex = 0; SectionIndex < LOD.Sections.Num(); ++SectionIndex)
				{
					FStaticMeshSection& Section = LOD.Sections[SectionIndex];
					Section.MaterialIndex = MaterialID;
					Section.bEnableCollision = true;
					Section.bCastShadow = true;
					Section.bForceOpaque = true;
					MaterialID++;
				}
				
			}
			
			for(int i = 0; i < Model->Meshes.Num(); i++)
			{
				FJJMesh& Mesh = Model->Meshes[i];
				
				UMaterialInstanceDynamic* DynamicMat = Mesh.Material.Opacity == 1 ?
					UMaterialInstanceDynamic::Create(DynamicMaterialDiffuse, this) :
					UMaterialInstanceDynamic::Create(DynamicMaterialOpacity, this);

				DynamicMat->SetVectorParameterValue("DiffuseColor",  Mesh.Material.DiffuseColor);
				DynamicMat->SetScalarParameterValue("Opacity",  Mesh.Material.Opacity);
								
				if(IsValid(Mesh.Material.Texture))
				 	DynamicMat->SetTextureParameterValue("TextureD",  Mesh.Material.Texture);

				StaticMeshComponent->SetMaterial(i, DynamicMat);
			}

			StaticMesh->MarkPackageDirty();
				
			StaticMeshComponent->SetMobility(EComponentMobility::Movable);
			StaticMeshComponent->SetGenerateOverlapEvents(true);
			StaticMeshComponent->SetCollisionResponseToChannels(ECollisionResponse::ECR_Overlap);
			StaticMeshComponent->SetCollisionResponseToChannel(ECollisionChannel::ECC_Visibility, ECollisionResponse::ECR_Block);

			UCollisionProvider* Provider = NewObject<UCollisionProvider>();
			Provider->Init(this);
			UBodySetup* BodySetup = NewObject<UBodySetup>(Provider, NAME_None, (IsTemplate() ? RF_Public : RF_NoFlags));
			
			BodySetup->BodySetupGuid = FGuid::NewGuid();
			
			BodySetup->bGenerateMirroredCollision = false;
			BodySetup->bDoubleSidedGeometry = true;
			BodySetup->CollisionTraceFlag = CTF_UseSimpleAndComplex;

			auto& BodyConvex = BodySetup->AggGeom.ConvexElems.AddDefaulted_GetRef();
			for(FJJMesh Mesh : Model->Meshes)
			{
				BodyConvex.VertexData.Append(Mesh.Geometry.Positions);
			}
			
			BodyConvex.UpdateElemBox();
			BodyConvex.SetContributeToMass(false);
			
			BodySetup->CreatePhysicsMeshesAsync(
				FOnAsyncPhysicsCookFinished::CreateUObject(this, &AJJRuntimeMesh::FinishPhysicsAsyncCook, BodySetup));
		});

	});
}

void AJJRuntimeMesh::FinishPhysicsAsyncCook(bool bSuccess, UBodySetup* FinishedBodySetup)
{
	check(IsInGameThread());
	StaticMesh->SetBodySetup(FinishedBodySetup);
	StaticMeshComponent->RecreatePhysicsState();

	if(!bSaveModelInMemory)
		Model.Reset();

	OnRuntimeMeshRebuild.Broadcast(bSuccess);
	OnRuntimeMeshRebuildStatic.Broadcast(bSuccess);
}

