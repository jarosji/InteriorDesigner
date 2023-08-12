// Fill out your copyright notice in the Description page of Project Settings.


#include "Room/JJRoom.h"

#include "DynamicMeshToMeshDescription.h"
#include "StaticMeshAttributes.h"
#include "Components/SplineComponent.h"
#include "GeometryScript/MeshMaterialFunctions.h"
#include "GeometryScript/MeshPrimitiveFunctions.h"


// Sets default values
AJJRoom::AJJRoom()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	
	Spline = CreateDefaultSubobject<USplineComponent>(TEXT("Spline"));
	Spline->SetupAttachment(DynamicMeshComponent);
}

void AJJRoom::OnConstruction(const FTransform& Transform)
{
	Super::OnConstruction(Transform);

	//Rebuild();
}

RoomSaveData AJJRoom::CreateSaveData()
{
	RoomSaveData Save;
	FMeshDescription MeshDescription;

	FDynamicMeshToMeshDescription Converter;
	FStaticMeshAttributes StaticMeshAttributes(MeshDescription);
	StaticMeshAttributes.Register();
	
	auto A = GetDynamicMeshComponent()->GetDynamicMesh()->GetMeshPtr();
	Converter.Convert(A, MeshDescription);

	for(FVertexID Id : MeshDescription.Vertices().GetElementIDs())
	{
		Save.Vertices.Add(FVector(MeshDescription.GetVertexPosition(Id)));
	}
	for(FTriangleID Id : MeshDescription.Triangles().GetElementIDs())
	{
		TArrayView<const FVertexID> VertIds = MeshDescription.GetTriangleVertices(Id);
		Save.Triangles.Add(VertIds[0]);
		Save.Triangles.Add(VertIds[1]);
		Save.Triangles.Add(VertIds[2]);
	}

	Save.Transform = GetTransform();
	
	return Save;
}

void AJJRoom::BuildMeshDesc(RoomSaveData& RoomSaveData)
{
	TFunctionRef<void(FDynamicMesh3&)> Func = [&RoomSaveData] (FDynamicMesh3& Mesh)
	{
		for(FVector Vec : RoomSaveData.Vertices)
		{
			Mesh.AppendVertex(Vec);
		}

		Mesh.EnableVertexColors(FVector3f::Zero());
		for (size_t vi = 0; vi < RoomSaveData.Vertices.Num() / 3; ++vi)
		{
			Mesh.SetVertexColor(vi, FVector3f(1, 1, 1));
		}

		for(int i =0;i<RoomSaveData.Triangles.Num();i+=3)
		{
			FVertexID Index0 = RoomSaveData.Triangles[i];
			FVertexID Index1 = RoomSaveData.Triangles[i+1];
			FVertexID Index2 = RoomSaveData.Triangles[i+2];
			Mesh.AppendTriangle(Index0,Index1,Index2);		
		}
	};
	
	GetDynamicMeshComponent()->GetDynamicMesh()->EditMesh(Func, EDynamicMeshChangeType::GeneralEdit);
}

void AJJRoom::Rebuild()
{
	FloorPoints.Empty();
	
	DynamicMesh = DynamicMeshComponent->GetDynamicMesh();
	if(!IsValid(DynamicMesh))
		return;

	DynamicMesh->Reset();


	TArray<UMaterialInterface*> NewMaterialSet;
	NewMaterialSet.Add(FloorMaterial);
	NewMaterialSet.Add(InsideMaterial);
	NewMaterialSet.Add(OutsideMaterial);
	NewMaterialSet.Add(UMaterial::GetDefaultMaterial(MD_Surface));
	DynamicMeshComponent->ConfigureMaterialSet(NewMaterialSet);

	const FVector& Location = Spline->GetLocationAtSplinePoint(0, ESplineCoordinateSpace::Local);
	const FRotator& Rotation = Spline->GetRotationAtSplinePoint(0, ESplineCoordinateSpace::Local);

	CachedVecInside = Location + FVector(0, -Thickness/2, 0).RotateAngleAxis(Rotation.Yaw, FVector(0,0,1));
	CachedVecOutside = Location + FVector(0, Thickness/2, 0).RotateAngleAxis(Rotation.Yaw, FVector(0,0,1));

	FloorPoints.Add(FVector2D(CachedVecInside.X, CachedVecInside.Y));
	for (int i = 0;i<Spline->GetNumberOfSplinePoints()-1;i++)
	{
		AddWall(CachedVecInside, CachedVecOutside, i);
	}

	AddFloor();

	//CalcCollision();
}

void AJJRoom::SetMaterial(ERoomMaterialType RoomType, UMaterialInstance* Material)
{
	switch (RoomType)
	{
		case ERoomMaterialType::Floor : FloorMaterial = Material; break;
		case ERoomMaterialType::WallOutside : OutsideMaterial = Material; break;
		case ERoomMaterialType::WallInside : InsideMaterial = Material; break;
	}

	Rebuild();
}

void AJJRoom::SetHeight(float NewHeight)
{
	Height = NewHeight;
	Rebuild();
}

void AJJRoom::SetThickness(float NewThickness)
{
	Height = NewThickness;
	Rebuild();
}

USplineComponent* AJJRoom::GetSpline()
{
	return Spline;
}

// Called when the game starts or when spawned
void AJJRoom::BeginPlay()
{
	Super::BeginPlay();
}

// Called every frame
void AJJRoom::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void AJJRoom::AddWall(const FVector StartInside, const FVector StartOutside, int Index)
{
	FVector Intersection;
	if(CalculateWallEndCoord(StartInside,  -Thickness/2, Index, Intersection))
	{
		AddWallSegment(StartInside, Intersection, Spline->GetRotationAtSplinePoint(Index, ESplineCoordinateSpace::Local).Yaw, false);
		CachedVecInside = Intersection;
		FloorPoints.Add(FVector2D(Intersection.X, Intersection.Y));
	}

	if(CalculateWallEndCoord(StartOutside, Thickness/2, Index, Intersection))
	{
		AddWallSegment(StartOutside, Intersection, Spline->GetRotationAtSplinePoint(Index, ESplineCoordinateSpace::Local).Yaw, true);
		CachedVecOutside= Intersection;
	}

	// Add top of a wall
	{
		TArray<FVector2D> PolygonVerts;
		PolygonVerts.Reserve(4);
		PolygonVerts.Add(FVector2D(StartInside.X, StartInside.Y));
		PolygonVerts.Add(FVector2D(CachedVecInside.X, CachedVecInside.Y));
		PolygonVerts.Add(FVector2D(CachedVecOutside.X, CachedVecOutside.Y));
		PolygonVerts.Add(FVector2D(StartOutside.X, StartOutside.Y));
		
		FTransform Transform;
		Transform.SetLocation(FVector(0,0,Height));
		
		UGeometryScriptLibrary_MeshPrimitiveFunctions::AppendTriangulatedPolygon(
		DynamicMesh,
		FGeometryScriptPrimitiveOptions(),
		Transform,
		PolygonVerts, true);

		UGeometryScriptLibrary_MeshMaterialFunctions::RemapMaterialIDs(DynamicMesh, 0, 3);
	}
}

void AJJRoom::AddWallSegment(const FVector& Start, const FVector& End, float Rot, bool FlipOrientation)
{
	float Distance = FVector::Distance(Start, End);
	FVector A = FVector(Distance/2 + Start.X, Start.Y, 0) - Start;
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
	Distance, Height,
	0, 0);

	int ToId = FlipOrientation ? 2 : 1;
	UGeometryScriptLibrary_MeshMaterialFunctions::RemapMaterialIDs(DynamicMesh, 0, ToId);
}

bool AJJRoom::CalculateWallEndCoord(const FVector& Start, const float Offset, int Index, FVector& OutIntersection)
{
	bool bIsPreLast = Index + 2 == Spline->GetNumberOfSplinePoints() - 1;

	bool bIsInSnapDistance;
	{
		const FVector& Location1 = Spline->GetLocationAtSplinePoint(Index, ESplineCoordinateSpace::Local);
		const FVector& Location2 = Spline->GetLocationAtSplinePoint(Spline->GetNumberOfSplinePoints()-1, ESplineCoordinateSpace::Local);
		const FVector& Location3 = Spline->GetLocationAtSplinePoint(Spline->GetNumberOfSplinePoints()-2, ESplineCoordinateSpace::Local);
		bIsInSnapDistance = FMath::PointDistToSegment(Location2, Location1, Location3) <= SnapDistance;
	}

	bool bIsLastSegment = Index + 2 < Spline->GetNumberOfSplinePoints(); 
	//Calculate fill
	if(!(bIsPreLast && bIsInSnapDistance) && bIsLastSegment)
	{
		const FVector& Location1 = Spline->GetLocationAtSplinePoint(Index + 1, ESplineCoordinateSpace::Local);
		const FRotator& Rotation1 = Spline->GetRotationAtSplinePoint(Index, ESplineCoordinateSpace::Local);
		const FVector& Location2 = Spline->GetLocationAtSplinePoint(Index + 2, ESplineCoordinateSpace::Local);
		const FRotator& Rotation2 = Spline->GetRotationAtSplinePoint(Index + 1, ESplineCoordinateSpace::Local);

		float Distance1 = FVector::Distance(Start, Location1);
		float Distance2 = FVector::Distance(Location1, Location2);

		const FVector& Rotated1 = FVector(Distance1 * 2, 0, 0).RotateAngleAxis(Rotation1.Yaw, FVector(0,0,1));
		const FVector& Rotated2 = FVector(Distance2 * -2, Offset, 0).RotateAngleAxis(Rotation2.Yaw, FVector(0,0,1));
		const FVector& Rotated3 = FVector(0, Offset, 0).RotateAngleAxis(Rotation2.Yaw, FVector(0,0,1));

		return FMath::SegmentIntersection2D(Start, Start + Rotated1,
			Location2 + Rotated3, Location2 + Rotated2,
			OutIntersection);
	}

	//Calculate partial
	const FVector& Location = Spline->GetLocationAtSplinePoint(Index + 1, ESplineCoordinateSpace::Local);
	const FRotator& Rotation = Spline->GetRotationAtSplinePoint(Index, ESplineCoordinateSpace::Local);
	OutIntersection = Location + FVector(0, Offset, 0).RotateAngleAxis(Rotation.Yaw, FVector(0,0,1));

	return true;
}

void AJJRoom::AddFloor()
{
	UGeometryScriptLibrary_MeshPrimitiveFunctions::AppendTriangulatedPolygon(
		DynamicMesh,
		FGeometryScriptPrimitiveOptions(),
		FTransform(),
		FloorPoints, true);
}

void AJJRoom::SetupCollision()
{
	
}

