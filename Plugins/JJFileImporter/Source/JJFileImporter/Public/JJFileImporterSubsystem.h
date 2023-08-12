// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintAsyncActionBase.h"
#include "UObject/Object.h"
#include "Wrappers/JJModelWrapper.h"
#include "JJFileImporterSubsystem.generated.h"

struct FJJModel;

DECLARE_DELEGATE_TwoParams(FOnModelImportedStatic, TSharedPtr<FJJModel> ImportedModel, bool bIsSuccess);
DECLARE_DELEGATE_TwoParams(FOnTextureImportedStatic, UTexture2D* Texture, bool bIsSuccess);

UCLASS()
class JJFILEIMPORTER_API UJJFileImporterSubsystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()

public:
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;
	virtual void Deinitialize() override;

public:
	void ImportModelAsync(const FString& FilePath, FOnModelImportedStatic OnModelImported);

#pragma region TextureImport
public:
	void ImportTextureAsync(const FString& FilePath, FOnTextureImportedStatic OnTextureImported);
	bool ImportTexture(const FString& FilePath, UTexture2D*& OutTexture);

private:
	UPROPERTY() TMap<FName, UTexture2D*> TexturesInImport;
	
#pragma endregion
};



// ====== Latent Imports - Exclusive for BPs ======

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnJJModelImported, const UJJModelWrapper*, ModelWrapper, bool, bIsSuccess);

UCLASS()
class JJFILEIMPORTER_API UImportModelAsync : public UBlueprintAsyncActionBase
{
	GENERATED_BODY()

public:
	UPROPERTY(BlueprintAssignable)
	FOnJJModelImported OnJJModelImported;

	UFUNCTION(BlueprintCallable, meta = (BlueprintInternalUseOnly = "true", WorldContext = "InWorldContextObj"), Category = "JJImporter")
	static UImportModelAsync* ImportModelAsync(const UObject* InWorldContextObj, const FString& InFilePath);

	//Overriding BP async action base
	virtual void Activate() override;

private:
	FString FilePath;
	const UObject* WorldContext;
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnTextureImported, const UTexture2D*, Texture, bool, bIsSuccess);

UCLASS()
class JJFILEIMPORTER_API UImportTextureAsync : public UBlueprintAsyncActionBase
{
	GENERATED_BODY()

public:
	UPROPERTY(BlueprintAssignable)
	FOnTextureImported OnTextureImported;

	UFUNCTION(BlueprintCallable, meta = (BlueprintInternalUseOnly = "true", WorldContext = "InWorldContextObj"), Category = "JJImporter")
	static UImportTextureAsync* ImportTextureAsync(const UObject* InWorldContextObj, const FString& InFilePath);

	//Overriding BP async action base
	virtual void Activate() override;

private:
	FString FilePath;
	UPROPERTY() UTexture2D* ImportedTexture;

	const UObject* WorldContext;
};
