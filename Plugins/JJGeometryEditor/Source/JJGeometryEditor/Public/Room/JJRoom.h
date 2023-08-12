// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "DynamicMeshActor.h"
#include "GameFramework/Actor.h"
#include "JJRoom.generated.h"

class USplineComponent;

UENUM()
enum ERoomMaterialType
{
	WallInside,
	WallOutside,
	Floor
};

struct RoomSaveData
{
	TArray<FVector> Vertices;
	TArray<FVertexID> Triangles;
	
	FTransform Transform;
	
	FORCEINLINE friend  FArchive& operator<<(FArchive &Ar, RoomSaveData& SaveGameData ) {
		Ar << SaveGameData.Vertices;
		Ar << SaveGameData.Triangles;
		Ar << SaveGameData.Transform;
		
		return Ar;
	}	
};

UCLASS()
class JJGEOMETRYEDITOR_API AJJRoom : public ADynamicMeshActor
{
	GENERATED_BODY()

private:
	constexpr static float SnapDistance = 20;

public:
	// Sets default values for this actor's properties
	AJJRoom();

	virtual void OnConstruction(const FTransform& Transform) override;

	RoomSaveData CreateSaveData();
	void BuildMeshDesc(RoomSaveData& RoomSaveData);

	UFUNCTION(BlueprintCallable)
	void Rebuild();

	UFUNCTION(BlueprintCallable)
	void SetMaterial(ERoomMaterialType RoomType, UMaterialInstance* Material);

	UFUNCTION(BlueprintCallable)
	void SetHeight(float NewHeight);

	UFUNCTION(BlueprintCallable)
	void SetThickness(float NewThickness);

	UFUNCTION(BlueprintPure)
	USplineComponent* GetSpline();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UPROPERTY(BlueprintReadWrite)
	UDynamicMesh* DynamicMesh;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	USplineComponent* Spline;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	UMaterialInstance* InsideMaterial;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	UMaterialInstance* OutsideMaterial;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	UMaterialInstance* FloorMaterial;
	
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	UMaterialInstance* WallTopMaterial;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	float Height = 200;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	float Thickness = 50;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

private:
	FVector CachedVecInside;
	FVector CachedVecOutside;
	TArray<FVector2d> FloorPoints;
	
	void AddWall(const FVector StartInside, const FVector StartOutside, int Index);
	void AddWallSegment(const FVector& Start, const FVector& End, float Rot, bool FlipOrientation);
	bool CalculateWallEndCoord(const FVector& Start, const float Offset, int Index, FVector& OutIntersection);
	
	void AddFloor();
	void SetupCollision();
};
