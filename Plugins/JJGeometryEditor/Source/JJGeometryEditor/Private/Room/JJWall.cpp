// Fill out your copyright notice in the Description page of Project Settings.


#include "Room/JJWall.h"

#include "GeometryScript/CollisionFunctions.h"
#include "GeometryScript/MeshMaterialFunctions.h"
#include "GeometryScript/MeshPrimitiveFunctions.h"
#include "Room/JJRoom.h"


// Sets default values
AJJWall::AJJWall()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;
}

FJJWallSaveData AJJWall::CreateSaveData()
{
	return {InnerTexturePath, OuterTexturePath};
}

// Called when the game starts or when spawned
void AJJWall::BeginPlay()
{
	Super::BeginPlay();
}

// Called every frame
void AJJWall::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	Rebuild();
}

void AJJWall::Rebuild()
{
	TArray<UMaterialInterface*> NewMaterialSet;
	NewMaterialSet.Add(UMaterial::GetDefaultMaterial(MD_Surface));
	NewMaterialSet.Add(InsideMaterial);
	NewMaterialSet.Add(OutsideMaterial);
	DynamicMeshComponent->ConfigureMaterialSet(NewMaterialSet);
	UGeometryScriptLibrary_MeshMaterialFunctions::EnableMaterialIDs(DynamicMesh);
}

void AJJWall::AddWallSegment(const FVector& Start, const FVector& End, float Rot, bool FlipOrientation)
{
	float Width = FVector::Distance(Start, End);
	FVector A = FVector(Width/2 + Start.X, Start.Y, 0) - Start;
	FVector Location = Start + FRotator(0, Rot, 0).RotateVector(A);
	Location.Z = Height/2;
	
	FGeometryScriptPrimitiveOptions PrimitiveOptions;
	PrimitiveOptions.bFlipOrientation = FlipOrientation;
	FTransform Transform;
	Transform.SetLocation(Location);
	Transform.SetRotation(FRotator(0, Rot, 270).Quaternion());
	
	UGeometryScriptLibrary_MeshPrimitiveFunctions::AppendRectangleXY(
	DynamicMesh,
	PrimitiveOptions,
	Transform,
	Width, Height,
	0, 0);

	int ToId = FlipOrientation ? 2 : 1;
	UGeometryScriptLibrary_MeshMaterialFunctions::RemapMaterialIDs(DynamicMesh, 0, ToId);
}

void AJJWall::AddWallTop(TArray<FVector2D>& PolygonVerts)
{		
	FTransform Transform;
	Transform.SetLocation(FVector(0,0,Height));
		
	UGeometryScriptLibrary_MeshPrimitiveFunctions::AppendTriangulatedPolygon(
	DynamicMesh,
	FGeometryScriptPrimitiveOptions(),
	Transform,
	PolygonVerts, true);

	FGeometryScriptCollisionFromMeshOptions Options;
	Options.Method = EGeometryScriptCollisionGenerationMethod::OrientedBoxes;
	UGeometryScriptLibrary_CollisionFunctions::SetDynamicMeshCollisionFromMesh(DynamicMesh, DynamicMeshComponent, Options);
}

void AJJWall::Restart()
{
	DynamicMesh = DynamicMeshComponent->GetDynamicMesh();
	DynamicMesh->Reset();
}

void AJJWall::Init(UMaterialInterface* MaterialInterface)
{
	InsideMaterial = UMaterialInstanceDynamic::Create(MaterialInterface, this);
	OutsideMaterial = UMaterialInstanceDynamic::Create(MaterialInterface, this);
	WallTopMaterial = UMaterialInstanceDynamic::Create(MaterialInterface, this);
}

void AJJWall::SetTexture(ERoomMaterialType RoomType, UTexture2D* InTexture, const FString& TexturePath)
{
	switch (RoomType)
	{
		case ERoomMaterialType::WallOutside :
			{
				OuterTexturePath = TexturePath;
				OutsideMaterial->SetTextureParameterValue("Texture", InTexture);
				break;
			}
		case ERoomMaterialType::WallInside :
			{
				InnerTexturePath = TexturePath;
				InsideMaterial->SetTextureParameterValue("Texture", InTexture);
				break;
			}
	}
	
	Rebuild();
}

void AJJWall::SetHeight(float NewHeight)
{
	Height = NewHeight;
}

void AJJWall::SetThickness(float NewThickness)
{
	Thickness = NewThickness;
}

