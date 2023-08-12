// Fill out your copyright notice in the Description page of Project Settings.


#include "JJFileImporterSubsystem.h"

#include "Importers/ModelImporter/JJModelImporter.h"
#include "Importers/TextureImporter/JJTextureImporter.h"
#include "Wrappers/JJModelWrapper.h"

void UJJFileImporterSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);
}

void UJJFileImporterSubsystem::Deinitialize()
{
	Super::Deinitialize();
}

void UJJFileImporterSubsystem::ImportModelAsync(const FString& FilePath, FOnModelImportedStatic OnModelImported)
{
	AsyncTask(ENamedThreads::AnyThread, [this, FilePath, OnModelImported]
	{
		JJModelImporter Importer(FilePath);
		TSharedRef<FJJModel> ImportedModel =  Importer.ImportModel();
		
		AsyncTask(ENamedThreads::GameThread, [this, ImportedModel, OnModelImported]
		{
			OnModelImported.Execute(ImportedModel, true);
		});
	});
}

void UJJFileImporterSubsystem::ImportTextureAsync(const FString& FilePath, FOnTextureImportedStatic OnTextureImported)
{
	if(!FPaths::FileExists(FilePath))
	{
		OnTextureImported.ExecuteIfBound(nullptr, false);
		return;
	}

	TexturesInImport.Add(FName(*FilePath), NewObject<UTexture2D>());
	AsyncTask(ENamedThreads::AnyThread, [FilePath, this, OnTextureImported]
		{
			JJTextureImporter LoadTextureAsync;
			LoadTextureAsync.ImportTexture(FilePath, TexturesInImport[FName(*FilePath)]);

			AsyncTask(ENamedThreads::GameThread, [FilePath, this, OnTextureImported] ()
			{
				if(TexturesInImport.Contains(FName(*FilePath)))
				{
					OnTextureImported.ExecuteIfBound(TexturesInImport[FName(*FilePath)], true);
					TexturesInImport.Remove(FName(*FilePath));
				}
			});
		});
}

bool UJJFileImporterSubsystem::ImportTexture(const FString& FilePath, UTexture2D*& OutTexture)
{
	if(!FPaths::FileExists(FilePath))
	{
		return false;
	}
	JJTextureImporter LoadTextureAsync;
	LoadTextureAsync.ImportTexture(FilePath, OutTexture);

	return IsValid(OutTexture);
}

UImportModelAsync* UImportModelAsync::ImportModelAsync(const UObject* InWorldContextObj, const FString& InFilePath)
{
	UImportModelAsync* ModelImporter = NewObject<UImportModelAsync>();
	ModelImporter->WorldContext = InWorldContextObj;
	ModelImporter->FilePath = InFilePath;
	
	return ModelImporter;
}

void UImportModelAsync::Activate()
{
	FOnModelImportedStatic OnModelImportedStatic;
	OnModelImportedStatic.BindLambda([this] (const TSharedPtr<FJJModel> ImportedModel, bool bIsSuccess)
	{
		UJJModelWrapper* ResultWrapper = NewObject<UJJModelWrapper>(this);
		ResultWrapper->Init(ImportedModel);
		OnJJModelImported.Broadcast(ResultWrapper, bIsSuccess);
	});
	WorldContext->GetWorld()->GetGameInstance()->GetSubsystem<UJJFileImporterSubsystem>()->ImportModelAsync(FilePath, OnModelImportedStatic);
}

UImportTextureAsync* UImportTextureAsync::ImportTextureAsync(const UObject* InWorldContextObj, const FString& InFilePath)
{
	UImportTextureAsync* TextureImporter = NewObject<UImportTextureAsync>();
	TextureImporter->WorldContext = InWorldContextObj;
	TextureImporter->FilePath = InFilePath;
	
	return TextureImporter;
}

void UImportTextureAsync::Activate()
{
	FOnTextureImportedStatic OnTextureImportedStatic;
	OnTextureImportedStatic.BindLambda([this] (const UTexture2D* Texture, bool bIsSuccess)
	{
		OnTextureImported.Broadcast(Texture, bIsSuccess);
	});
	
	WorldContext->GetWorld()->GetGameInstance()->GetSubsystem<UJJFileImporterSubsystem>()->ImportTextureAsync(FilePath, OnTextureImportedStatic);
}
