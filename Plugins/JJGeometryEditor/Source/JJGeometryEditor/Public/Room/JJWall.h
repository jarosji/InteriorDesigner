// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "DynamicMeshActor.h"
#include "GameFramework/Actor.h"
#include "Room/JJRoom.h"
#include "JJWall.generated.h"

USTRUCT()
struct FJJWallSaveData
{
	GENERATED_BODY()

	FString InnerTexturePath;
	FString OuterTexturePath;

	FORCEINLINE friend  FArchive& operator<<(FArchive &Ar, FJJWallSaveData& SaveGameData ) {
		Ar << SaveGameData.InnerTexturePath;
		Ar << SaveGameData.OuterTexturePath;
		
		return Ar;
	}	
};

UCLASS()
class JJGEOMETRYEDITOR_API AJJWall : public ADynamicMeshActor
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	AJJWall();

	FJJWallSaveData CreateSaveData();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	void Rebuild();

	void AddWallSegment(const FVector& Start, const FVector& End, float Rot, bool FlipOrientation);
	void AddWallTop(TArray<FVector2D>& PolygonVerts);

	void Restart();
	void Init(UMaterialInterface* MaterialInterface);

	UFUNCTION(BlueprintCallable)
	void SetTexture(ERoomMaterialType RoomType, UTexture2D* InTexture, const FString& TexturePath);

	UFUNCTION(BlueprintCallable)
	void SetHeight(float NewHeight);

	UFUNCTION(BlueprintCallable)
	void SetThickness(float NewThickness);

protected:
	
	UPROPERTY(BlueprintReadWrite)
	UDynamicMesh* DynamicMesh;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	UMaterialInstanceDynamic* InsideMaterial;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	UMaterialInstanceDynamic* OutsideMaterial;
	
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	UMaterialInstanceDynamic* WallTopMaterial;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	float Height = 200;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	float Thickness = 50;

private:
	FString InnerTexturePath;
	FString OuterTexturePath;
};
