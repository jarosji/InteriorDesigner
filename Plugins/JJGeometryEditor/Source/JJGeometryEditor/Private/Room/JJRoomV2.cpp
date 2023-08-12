// Fill out your copyright notice in the Description page of Project Settings.


#include "Room/JJRoomV2.h"

#include "DynamicMeshToMeshDescription.h"
#include "StaticMeshAttributes.h"
#include "Components/SplineComponent.h"
#include "GeometryScript/CollisionFunctions.h"
#include "GeometryScript/MeshPrimitiveFunctions.h"
#include "Room/JJWall.h"


// Sets default values
AJJRoomV2::AJJRoomV2()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	
	Spline = CreateDefaultSubobject<USplineComponent>(TEXT("Spline"));
	Spline->SetupAttachment(DynamicMeshComponent);

	Spline->SetSplinePointType(0, ESplinePointType::Linear);
	Spline->SetSplinePointType(1, ESplinePointType::Linear);

	{
		const FString& Path = "/Script/Engine.Material'/JJGeometryEditor/Materials/M_RoomMaterial.M_RoomMaterial'";
		ConstructorHelpers::FObjectFinder<UMaterialInterface> Mat(*Path);
		DefaultMaterial = Mat.Object;
	}
}

JJRoomSaveData AJJRoomV2::CreateSaveData()
{
	JJRoomSaveData Save;
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
	Save.FloorTexturePath = FloorTexturePath;

	for (int i = 0;i<Spline->GetNumberOfSplinePoints();i++)
	{
		Save.SplinePoints.Add(Spline->GetLocationAtSplinePoint(i, ESplineCoordinateSpace::Local));
	}

	for (AJJWall* Wall : WallPool)
	{
		Save.WallSaves.Add(Wall->CreateSaveData());
	}
	
	return Save;
}

void AJJRoomV2::BuildMeshDesc(JJRoomSaveData& RoomSaveData)
{
	for (int i = 0;i < RoomSaveData.SplinePoints.Num();i++)
	{
		if(i<=1)
		{
			Spline->SetLocationAtSplinePoint(i, RoomSaveData.SplinePoints[i], ESplineCoordinateSpace::Local);
			continue;
		}
		
		FSplinePoint Point;
		Point.Position = RoomSaveData.SplinePoints[i];
		Point.Type = ESplinePointType::Linear;
		Point.InputKey = i;
		Spline->AddPoint(Point);

		/*
		auto act = GetWorld()->SpawnActor<AJJWall>();
		WallPool.Add(act);
		act->Init(DefaultMaterial);*/
	}
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
	//GetDynamicMeshComponent()->GetDynamicMesh()->EditMesh(Func, EDynamicMeshChangeType::GeneralEdit);
}

void AJJRoomV2::Rebuild()
{
	FloorPoints.Empty();
	
	DynamicMesh = DynamicMeshComponent->GetDynamicMesh();
	if(!IsValid(DynamicMesh))
		return;

	DynamicMesh->Reset();

	TArray<UMaterialInterface*> NewMaterialSet;
	NewMaterialSet.Add(FloorMaterial);
	DynamicMeshComponent->ConfigureMaterialSet(NewMaterialSet);


	{
		const FVector& Location = Spline->GetLocationAtSplinePoint(0, ESplineCoordinateSpace::Local);
		const FRotator& Rotation = Spline->GetRotationAtSplinePoint(0, ESplineCoordinateSpace::Local);

		CachedVecInside = Location + FVector(0, -Thickness/2, 0).RotateAngleAxis(Rotation.Yaw, FVector(0,0,1));
		CachedVecOutside = Location + FVector(0, Thickness/2, 0).RotateAngleAxis(Rotation.Yaw, FVector(0,0,1));

		if(bIsClosedLoop)
		{
			CachedVecInside = CalculateClosedLoopVector(-Thickness/2);
			CachedVecOutside = CalculateClosedLoopVector(Thickness/2);
			CachedFirstVecInside = CachedVecInside;
			CachedFirstVecOutside = CachedVecOutside;
		}
	}

	FloorPoints.Add(FVector2D(CachedVecInside.X, CachedVecInside.Y));
	for (int i = 0;i<Spline->GetNumberOfSplinePoints()-1;i++)
	{
		AddWall(CachedVecInside, CachedVecOutside, i);
	}

	AddFloor();

	FGeometryScriptCollisionFromMeshOptions Options;
	Options.Method = EGeometryScriptCollisionGenerationMethod::SweptHulls;
	UGeometryScriptLibrary_CollisionFunctions::SetDynamicMeshCollisionFromMesh(DynamicMesh, DynamicMeshComponent, Options);
}

void AJJRoomV2::SetFloorTexture(UTexture2D* InTexture, const FString& TexturePath)
{
	FloorTexturePath = TexturePath;
	FloorMaterial->SetTextureParameterValue("Texture", InTexture);
	Rebuild();
}

USplineComponent* AJJRoomV2::GetSpline()
{
	return Spline;
}

void AJJRoomV2::SetClosedLoop(bool bIsClosed)
{
	bIsClosedLoop = bIsClosed;
	//Spline->SetClosedLoop(bIsClosedLoop);
	Rebuild();
}

// Called when the game starts or when spawned
void AJJRoomV2::BeginPlay()
{
	Super::BeginPlay();

	FloorMaterial = UMaterialInstanceDynamic::Create(DefaultMaterial, this);
}

// Called every frame
void AJJRoomV2::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void AJJRoomV2::AddWall(const FVector StartInside, const FVector StartOutside, int Index)
{
	AJJWall* Wall;
	if(WallPool.IsValidIndex(Index))
	{
		Wall = WallPool[Index];
	} else
	{
		Wall = GetWorld()->SpawnActor<AJJWall>();
		WallPool.Add(Wall);
		Wall->Init(DefaultMaterial);
		Wall->SetHeight(Height);
		Wall->SetThickness(Thickness);
	}
	Wall->Restart();
	Wall->SetActorLocation(GetActorLocation());
	
	FVector2D TopIn;
	FVector2D TopOut;

	FVector Intersection;
	if(bIsClosedLoop && Index == Spline->GetNumberOfSplinePoints()-2)
	{
		Wall->AddWallSegment(StartInside, CachedFirstVecInside, Spline->GetRotationAtSplinePoint(Index, ESplineCoordinateSpace::Local).Yaw, false);
		TopIn = FVector2D(CachedFirstVecInside.X, CachedFirstVecInside.Y);
		//FloorPoints.Add(FVector2D(CachedFirstVecInside.X, CachedFirstVecInside.Y));
	}
	else if(CalculateWallEndCoord(StartInside,  -Thickness/2, Index, Intersection))
	{
		Wall->AddWallSegment(StartInside, Intersection, Spline->GetRotationAtSplinePoint(Index, ESplineCoordinateSpace::Local).Yaw, false);
		CachedVecInside = Intersection;
		FloorPoints.Add(FVector2D(Intersection.X, Intersection.Y));
		TopIn = FVector2D(CachedVecInside.X, CachedVecInside.Y);
	}

	if(bIsClosedLoop && Index == Spline->GetNumberOfSplinePoints()-2)
	{
		Wall->AddWallSegment(StartOutside, CachedFirstVecOutside, Spline->GetRotationAtSplinePoint(Index, ESplineCoordinateSpace::Local).Yaw, true);
		//FloorPoints.Add(FVector2D(CachedFirstVecInside.X, CachedFirstVecInside.Y));
		TopOut = FVector2D(CachedFirstVecOutside.X, CachedFirstVecOutside.Y);
	}
	else if(CalculateWallEndCoord(StartOutside, Thickness/2, Index, Intersection))
	{
		Wall->AddWallSegment(StartOutside, Intersection, Spline->GetRotationAtSplinePoint(Index, ESplineCoordinateSpace::Local).Yaw, true);
		CachedVecOutside= Intersection;
		TopOut = FVector2D(CachedVecOutside.X, CachedVecOutside.Y);
	}

	// Add top of a wall
	{
		TArray<FVector2D> PolygonVerts;
		PolygonVerts.Reserve(4);
		PolygonVerts.Add(FVector2D(StartInside.X, StartInside.Y));
		PolygonVerts.Add(TopIn);
		PolygonVerts.Add(TopOut);
		PolygonVerts.Add(FVector2D(StartOutside.X, StartOutside.Y));

		Wall->AddWallTop(PolygonVerts);
	}
}

bool AJJRoomV2::CalculateWallEndCoord(const FVector& Start, const float Offset, int Index, FVector& OutIntersection)
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

void AJJRoomV2::AddFloor()
{
	UGeometryScriptLibrary_MeshPrimitiveFunctions::AppendTriangulatedPolygon(
		DynamicMesh,
		FGeometryScriptPrimitiveOptions(),
		FTransform(),
		FloorPoints, true);
}

FVector AJJRoomV2::CalculateClosedLoopVector(float Offset)
{
	const FVector& Location0 = Spline->GetLocationAtSplinePoint(Spline->GetNumberOfSplinePoints() - 2, ESplineCoordinateSpace::Local);
	const FRotator& Rotation0 = Spline->GetRotationAtSplinePoint(Spline->GetNumberOfSplinePoints() - 2, ESplineCoordinateSpace::Local);
	const FVector& Location1 = Spline->GetLocationAtSplinePoint(0, ESplineCoordinateSpace::Local);
	const FRotator& Rotation1 = Spline->GetRotationAtSplinePoint(0, ESplineCoordinateSpace::Local);
	const FVector& Location2 = Spline->GetLocationAtSplinePoint(1, ESplineCoordinateSpace::Local);

	float Distance1 = FVector::Distance(Location0, Location1);
	float Distance2 = FVector::Distance(Location1, Location2);
        
	const FVector& Rotated1 = FVector(0, Offset, 0).RotateAngleAxis(Rotation0.Yaw, FVector(0,0,1));
	const FVector& Rotated2 = FVector(Distance1 * 2, Offset, 0).RotateAngleAxis(Rotation0.Yaw, FVector(0,0,1));
	const FVector& Rotated3 = FVector(Distance2 * -2, Offset, 0).RotateAngleAxis(Rotation1.Yaw, FVector(0,0,1));
	const FVector& Rotated4 = FVector(0, Offset, 0).RotateAngleAxis(Rotation1.Yaw, FVector(0,0,1));

	FVector A = Location0 + Rotated1;
	FVector B = Location0 + Rotated2;
	FVector C = Location2 + Rotated3;
	FVector D = Location2 + Rotated4;

	FVector OutIntersection;
	FMath::SegmentIntersection2D(A, B,D, C,OutIntersection);
	
	return OutIntersection;
}

