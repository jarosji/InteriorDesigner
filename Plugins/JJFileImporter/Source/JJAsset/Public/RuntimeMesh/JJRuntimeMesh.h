// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"

#include "JJRuntimeMesh.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnRuntimeMeshRebuild, bool, bSuccess);
DECLARE_MULTICAST_DELEGATE_OneParam(FOnRuntimeMeshRebuildStatic, bool bSuccess);

struct FRuntimeMeshActorSaveData
{
	FTransform Transform;
	FString ModelPath;
	
	FORCEINLINE friend  FArchive& operator<<(FArchive &Ar, FRuntimeMeshActorSaveData& SaveGameData) {
		Ar << SaveGameData.ModelPath;
		Ar << SaveGameData.Transform;
		
		return Ar;
	}
};

struct FJJModel;
class AJJRuntimeMesh;
class UJJModelWrapper;

UCLASS(Blueprintable)
class JJASSET_API UCollisionProvider : public UObject, public IInterface_CollisionDataProvider
{
	GENERATED_BODY()
public:
	void Init(AJJRuntimeMesh* InRuntimeMeshActor);

	// ==== IInterface_CollisionDataProvider ====
	virtual bool GetPhysicsTriMeshData(struct FTriMeshCollisionData* CollisionData, bool InUseAllTriData) override;
	virtual bool ContainsPhysicsTriMeshData(bool InUseAllTriData) const override;
	virtual bool WantsNegXTriMesh() override;
	virtual void GetMeshId(FString& OutMeshId) override;
	//	==== IInterface_CollisionDataProvider ====

	UPROPERTY()
	AJJRuntimeMesh* OwningActor;
};

UCLASS(Blueprintable, BlueprintType)
class JJASSET_API AJJRuntimeMesh : public AActor
{
	GENERATED_BODY()

public:
	AJJRuntimeMesh();
	void Init(TSharedPtr<FJJModel> InModel, bool bInIsCPUAccessible, bool bInSaveModelInMemory);

	FRuntimeMeshActorSaveData CreateSaveData() const;

protected:
	virtual void BeginPlay() override;

public:

	UPROPERTY(BlueprintAssignable)
	FOnRuntimeMeshRebuild OnRuntimeMeshRebuild;

	FOnRuntimeMeshRebuildStatic OnRuntimeMeshRebuildStatic;
	
	UFUNCTION(BlueprintCallable)
	void RebuildModel();

private:
	UFUNCTION() void FinishPhysicsAsyncCook(bool bSuccess, UBodySetup* FinishedBodySetup);

private:
	TSharedPtr<FJJModel> Model;
	
	bool bIsCPUAccessible;
	bool bSaveModelInMemory;

	UPROPERTY() TObjectPtr<UMaterialInterface> DynamicMaterialDiffuse;
	UPROPERTY() TObjectPtr<UMaterialInterface> DynamicMaterialOpacity;

	UPROPERTY()	UStaticMeshComponent* StaticMeshComponent;
	UPROPERTY(Transient) UStaticMesh* StaticMesh;

	friend class UCollisionProvider;
};
