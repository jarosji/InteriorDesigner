// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "DynamicMeshActor.h"
#include "JJWall.h"
#include "GameFramework/Actor.h"
#include "Room/JJRoom.h"
#include "JJRoomV2.generated.h"

struct JJRoomSaveData
{
	TArray<FVector> SplinePoints;
	
	TArray<FVector> Vertices;
	TArray<FVertexID> Triangles;

	TArray<FJJWallSaveData> WallSaves;
	
	FTransform Transform;
	FString FloorTexturePath;

	bool bClosedLoop;
	
	FORCEINLINE friend  FArchive& operator<<(FArchive &Ar, JJRoomSaveData& SaveGameData ) {
		Ar << SaveGameData.SplinePoints;
		Ar << SaveGameData.Vertices;
		Ar << SaveGameData.Triangles;
		Ar << SaveGameData.Transform;
		Ar << SaveGameData.FloorTexturePath;
		Ar << SaveGameData.WallSaves;
		Ar << SaveGameData.bClosedLoop;
		
		return Ar;
	}	
};

class USplineComponent;

UCLASS()
class JJGEOMETRYEDITOR_API AJJRoomV2 : public ADynamicMeshActor
{
	GENERATED_BODY()

private:
	constexpr static float SnapDistance = 20;

public:
	// Sets default values for this actor's properties
	AJJRoomV2();

	JJRoomSaveData CreateSaveData();
	void BuildMeshDesc(JJRoomSaveData& RoomSaveData);

	UFUNCTION(BlueprintCallable)
	void Rebuild();

	UFUNCTION(BlueprintCallable)
	void SetFloorTexture(UTexture2D* InTexture, const FString& TexturePath);

	UFUNCTION(BlueprintPure)
	USplineComponent* GetSpline();

	UFUNCTION(BlueprintPure)
	TArray<AJJWall*>& GetWalls() { return WallPool; }

	UFUNCTION(BlueprintCallable)
	void SetClosedLoop(bool bIsClosed);


protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UPROPERTY(BlueprintReadWrite)
	UDynamicMesh* DynamicMesh;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	USplineComponent* Spline;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	UMaterialInterface* DefaultMaterial;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	UMaterialInstanceDynamic* FloorMaterial;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, meta=(ExposeOnSpawn="true"))
	float Height = 200;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, meta=(ExposeOnSpawn="true"))
	float Thickness = 50;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

private:
	FVector CachedVecInside;
	FVector CachedVecOutside;
	FVector CachedFirstVecInside;
	FVector CachedFirstVecOutside;
	
	FString FloorTexturePath;
	
	TArray<FVector2d> FloorPoints;
	UPROPERTY() TArray<AJJWall*> WallPool;
	
	void AddWall(const FVector StartInside, const FVector StartOutside, int Index);
	bool CalculateWallEndCoord(const FVector& Start, const float Offset, int Index, FVector& OutIntersection);

	bool bIsClosedLoop;
	
	void AddFloor();

	FVector CalculateClosedLoopVector(float Offset);
};
