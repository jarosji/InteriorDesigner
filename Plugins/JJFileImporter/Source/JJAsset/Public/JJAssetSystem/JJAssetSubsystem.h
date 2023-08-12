// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ImportedFiles/JJModel.h"
#include "Kismet/BlueprintAsyncActionBase.h"
#include "UObject/Object.h"
#include "JJAssetSubsystem.generated.h"

class IJJAssetInterface;
class UJJModelWrapper;
class UJJAssetModelWrapper;
class AJJRuntimeMesh;
class AJJPreviewMaker;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnTextureLoadedMulti, UTexture2D*, Texture, FString, FilePath);

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnModelAssetLoadedMulti, UJJAssetModelWrapper*, ModelWrapper);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnModelAssetExportedMulti, UJJAssetModelWrapper*, ModelWrapper, bool, bIsSuccess);

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnPreviewStarted, AJJRuntimeMesh*, RuntimeActor);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnPreviewEnded, AJJRuntimeMesh*, RuntimeActor);

DECLARE_DELEGATE_OneParam(FOnRuntimeActorInitializedStatic, AJJRuntimeMesh* RuntimeActor);

UCLASS()
class JJASSET_API UJJAssetSubsystem : public UWorldSubsystem
{
	GENERATED_BODY()

public:
	inline static const FString Extension = ".IdAsset";

public:
	UJJAssetSubsystem();
	
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;
	virtual void Deinitialize() override;

public:
	void SpawnRuntimeActor(const FString& AssetPath, FVector Location, FOnRuntimeActorInitializedStatic OnRuntimeActorInitializedStatic);
	void SpawnRuntimeActor(const TSharedPtr<FJJModel> Model, FVector Location, FOnRuntimeActorInitializedStatic OnRuntimeActorInitializedStatic);

#pragma region Preview
public:
	UPROPERTY(BlueprintAssignable)
	FOnPreviewStarted OnPreviewStarted;

	UPROPERTY(BlueprintAssignable)
	FOnPreviewStarted OnPreviewEnded;

	UFUNCTION(BlueprintCallable)
	void StartPreview(AJJRuntimeMesh* ActorToPreview);

	UFUNCTION(BlueprintCallable)
	void EndPreview(AJJRuntimeMesh* ActorToPreview, const FString& SnaphotPath);
	
private:
	UPROPERTY() TObjectPtr<AJJPreviewMaker> PreviewMakerActor;
#pragma endregion
	

public:
	UPROPERTY(BlueprintAssignable)
	FOnTextureLoadedMulti OnTextureLoadedMulti;
	
	UPROPERTY(BlueprintAssignable)
	FOnModelAssetLoadedMulti OnModelAssetLoadedMulti;
	UPROPERTY(BlueprintAssignable)
	FOnModelAssetExportedMulti OnModelAssetExportedMulti;

	UFUNCTION(BlueprintCallable, Category="ImportSubsystem")
	bool ExportModelAsAssetAsync(UJJModelWrapper* ModelWrapper, const FString& NewFilePath, const FJJImportSettings& Settings);

	UFUNCTION(BlueprintCallable, Category="ImportSubsystem")
	void LoadModelAssetsFromFolderAsync(const FString& FolderPath, bool bIsRecursive);

	UFUNCTION(BlueprintPure, Category="ImportSubsystem")
	TArray<UJJAssetModelWrapper*> GetModelWrappers() const;

	UFUNCTION(BlueprintCallable, Category="ImportSubsystem")
	void AddModelAsset(UJJAssetModelWrapper* AssetToAdd);

	UFUNCTION(BlueprintCallable, Category="ImportSubsystem")
	void RemoveModelAsset(UJJAssetModelWrapper* AssetToDelete);

	UFUNCTION(BlueprintCallable, Category="ImportSubsystem")
	void LoadTexturesFromFolderAsync(const FString& FolderPath, TArray<FString>& Extensions, bool bIsRecursive);

	UFUNCTION(BlueprintCallable, Category="ImportSubsystem")
	void LoadTextureAsync(const FString& FilePath);

	UFUNCTION(BlueprintPure, Category="ImportSubsystem")
	bool FindTexture(const FString& TexturePath, UTexture2D*& Texture) const;

	UFUNCTION(BlueprintPure, Category="ImportSubsystem")
	TMap<FString, UTexture2D*> GetTextures() const;

	UPROPERTY() TObjectPtr<UMaterialInterface> DynamicMaterialDiffuse;
	UPROPERTY() TObjectPtr<UMaterialInterface> DynamicMaterialOpacity;

private:
	UPROPERTY() TArray<UJJAssetModelWrapper*> ModelAssets;
	UPROPERTY() TMap<FString, UTexture2D*> Textures;
};

// ====== Latent Spawners - Exclusive for BPs ======

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnRuntimeActorInitialized, const AJJRuntimeMesh*, RuntimeActor);
UCLASS()
class JJASSET_API USpawnRuntimeMeshActorAsync : public UBlueprintAsyncActionBase
{
	GENERATED_BODY()

public:
	UPROPERTY(BlueprintAssignable)
	FOnRuntimeActorInitialized OnRuntimeActorInitialized;

	UFUNCTION(BlueprintCallable, meta = (BlueprintInternalUseOnly = "true", WorldContext = "InWorldContextObj"), Category = "JJImporter")
	static USpawnRuntimeMeshActorAsync* SpawnRuntimeMeshActorAsync(const UObject* InWorldContextObj, const UJJModelWrapper* InModel,
		FVector InLocation);

	//Overriding BP async action base
	virtual void Activate() override;

private:
	UPROPERTY() const UJJModelWrapper* Model;
	const UObject* WorldContext;

	FVector Location;
};

UCLASS()
class JJASSET_API USpawnRuntimeMeshActorFromAssetWrapperAsync : public UBlueprintAsyncActionBase
{
	GENERATED_BODY()

public:
	UPROPERTY(BlueprintAssignable)
	FOnRuntimeActorInitialized OnRuntimeActorInitialized;

	UFUNCTION(BlueprintCallable, meta = (BlueprintInternalUseOnly = "true", WorldContext = "InWorldContextObj"), Category = "JJImporter")
	static USpawnRuntimeMeshActorFromAssetWrapperAsync* SpawnRuntimeMeshActorFromAssetWrapperAsync(const UObject* InWorldContextObj, const UJJAssetModelWrapper* AssetWrapper,
		FVector InLocation);

	//Overriding BP async action base
	virtual void Activate() override;

private:
	UPROPERTY() const UJJAssetModelWrapper* Model;
	const UObject* WorldContext;

	FVector Location;
};
