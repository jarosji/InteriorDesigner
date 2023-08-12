// Fill out your copyright notice in the Description page of Project Settings.


#include "JJAssetSystem/JJAssetSubsystem.h"

#include "ImageUtils.h"
#include "JJFileImporterSubsystem.h"
#include "Importers/JJAssetImporter.h"
#include "Wrappers/JJModelWrapper.h"
#include "JJAssetSystem/AssetWrappers/JJAssetModelWrapper.h"
#include "Kismet/GameplayStatics.h"
#include "PreviewMaker/JJPreviewMaker.h"
#include "RuntimeMesh/JJRuntimeMesh.h"

UJJAssetSubsystem::UJJAssetSubsystem()
{
	{
		const FString& Path = "/Script/Engine.Material'/JJFileImporter/Materials/IM_ProceduralMaterial.IM_ProceduralMaterial'";
		ConstructorHelpers::FObjectFinder<UMaterialInterface> Mat(*Path);
		DynamicMaterialDiffuse = Mat.Object;
	}

	{
		const FString& Path = "/Script/Engine.Material'/JJFileImporter/Materials/IM_ProceduralMaterial_Opacity.IM_ProceduralMaterial_Opacity'";
		ConstructorHelpers::FObjectFinder<UMaterialInterface> Mat(*Path);
		DynamicMaterialOpacity = Mat.Object;
	}
}

void UJJAssetSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);
}

void UJJAssetSubsystem::Deinitialize()
{
	Super::Deinitialize();
}

void UJJAssetSubsystem::SpawnRuntimeActor(const FString& AssetPath, FVector Location,
	FOnRuntimeActorInitializedStatic OnRuntimeActorInitializedStatic)
{
	AsyncTask(ENamedThreads::AnyThread, [this, AssetPath, Location, OnRuntimeActorInitializedStatic]
		{
			TSharedPtr<FJJModel> OutModel;
			if (JJAssetImporter::ImportJJModel(AssetPath, OutModel))
			{
				for(FJJMesh& Mesh : OutModel->Meshes)
				{
					Mesh.Material.Texture = NewObject<UTexture2D>();
					GetWorld()->GetGameInstance()->GetSubsystem<UJJFileImporterSubsystem>()->ImportTexture(Mesh.Material.DestPath, Mesh.Material.Texture);
				}
				
				AsyncTask(ENamedThreads::GameThread, [this, OutModel, Location, OnRuntimeActorInitializedStatic]
				{
					SpawnRuntimeActor(OutModel, Location, OnRuntimeActorInitializedStatic);
				});
			}	
		});
}

void UJJAssetSubsystem::SpawnRuntimeActor(const TSharedPtr<FJJModel> Model, FVector Location, FOnRuntimeActorInitializedStatic OnRuntimeActorInitializedStatic)
{
	AJJRuntimeMesh* SpawnedActor = GetWorld()->SpawnActor<AJJRuntimeMesh>();
	SpawnedActor->Init(Model, true, true);

	FDelegateHandle Handle = SpawnedActor->OnRuntimeMeshRebuildStatic.AddLambda([SpawnedActor, OnRuntimeActorInitializedStatic, Handle] (bool bSuccess)
	{
		SpawnedActor->OnRuntimeMeshRebuildStatic.Remove(Handle);
		OnRuntimeActorInitializedStatic.ExecuteIfBound(SpawnedActor);
	});
	
	const FJJImportSettings& Settings = Model->Settings;

	FTransform Transform = Settings.Transform;
	Transform.SetLocation(Location);
	SpawnedActor->SetActorTransform(Transform);
	
	SpawnedActor->RebuildModel();
}

void UJJAssetSubsystem::StartPreview(AJJRuntimeMesh* ActorToPreview)
{
	PreviewMakerActor = Cast<AJJPreviewMaker>(UGameplayStatics::GetActorOfClass(this, AJJPreviewMaker::StaticClass()));
	if(IsValid(PreviewMakerActor))
	{
		PreviewMakerActor->StartPreview(ActorToPreview);
		OnPreviewStarted.Broadcast(ActorToPreview);
	}
	else
	{
		GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Yellow, TEXT("Previewer not found"));	
	}
}

void UJJAssetSubsystem::EndPreview(AJJRuntimeMesh* ActorToPreview, const FString& SnaphotPath)
{
	PreviewMakerActor = Cast<AJJPreviewMaker>(UGameplayStatics::GetActorOfClass(this, AJJPreviewMaker::StaticClass()));
	if(IsValid(PreviewMakerActor))
	{
		PreviewMakerActor->EndPreviewWithSnapshot(ActorToPreview, SnaphotPath);
		OnPreviewEnded.Broadcast(ActorToPreview);
	}
	else
	{
		GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Yellow, TEXT("Previewer not found"));	
	}
}

bool UJJAssetSubsystem::ExportModelAsAssetAsync(UJJModelWrapper* ModelWrapper, const FString& NewFilePath,
	const FJJImportSettings& Settings)
{
	TSharedPtr<FJJModel> ModelToExport = ModelWrapper->GetWrappedModel();
    
	FString AssetPath = FPaths::SetExtension(FPaths::Combine(NewFilePath, FPaths::GetBaseFilename(ModelToExport->OriginalSrcPath)), Extension);
    
	AsyncTask(ENamedThreads::AnyThread, [this, AssetPath, ModelToExport, ModelWrapper, Settings]
	{
		ModelWrapper->SetImportSettings(Settings);
		if(JJAssetImporter::ExportJJModel(AssetPath,*ModelToExport))
		{
			AsyncTask(ENamedThreads::GameThread, [this, ModelWrapper, AssetPath]
			{
				UJJAssetModelWrapper* ModelAsset = NewObject<UJJAssetModelWrapper>(this);
				ModelAsset->Init(AssetPath);
				AddModelAsset(ModelAsset);
				OnModelAssetExportedMulti.Broadcast(ModelAsset, true);
			});
		}
	});
	
	return true;
}

void UJJAssetSubsystem::LoadModelAssetsFromFolderAsync(const FString& FolderPath, bool bIsRecursive)
{
	TArray<FString> FoundFiles;
	FString Dir = FolderPath;
	IFileManager::Get().FindFiles(FoundFiles, *Dir, *Extension);
	
	for(const FString& FoundFile : FoundFiles)
	{
		UJJAssetModelWrapper* ModelWrapper = NewObject<UJJAssetModelWrapper>(this);
		ModelWrapper->Init(FPaths::Combine(FolderPath, FoundFile));
					
		AddModelAsset(ModelWrapper);
	}
}

TArray<UJJAssetModelWrapper*> UJJAssetSubsystem::GetModelWrappers() const
{
	return ModelAssets;
}

void UJJAssetSubsystem::AddModelAsset(UJJAssetModelWrapper* AssetToAdd)
{
	ModelAssets.Add(AssetToAdd);
	OnModelAssetLoadedMulti.Broadcast(AssetToAdd);
}

void UJJAssetSubsystem::RemoveModelAsset(UJJAssetModelWrapper* AssetToDelete)
{
	ModelAssets.Remove(AssetToDelete);
}

void UJJAssetSubsystem::LoadTexturesFromFolderAsync(const FString& FolderPath, TArray<FString>& Extensions, bool bIsRecursive)
{
	TArray<FString> FoundFiles;
	FString Dir = FolderPath;
	for(FString& TextureExtension : Extensions)
	{
		IFileManager::Get().FindFiles(FoundFiles, *Dir, *TextureExtension);
	
		for(const FString& FoundFile : FoundFiles)
		{
			LoadTextureAsync(FPaths::Combine(FolderPath, FoundFile));
		}
	}
}

void UJJAssetSubsystem::LoadTextureAsync(const FString& FilePath)
{
	/*FOnTextureImportedStatic OnTextureImportedStatic;
	OnTextureImportedStatic.BindLambda([this, FilePath](UTexture2D* Texture, bool bIsSuccess)
	{
		if(bIsSuccess)
		{
			Textures.Add(FilePath, Texture);
			OnTextureLoadedMulti.Broadcast(Texture, FilePath);
		}
	});
	GetGameInstance()->GetSubsystem<UJJFileImporterSubsystem>()->ImportTextureAsync(FilePath, OnTextureImportedStatic);*/

	UTexture2D* Texture = FImageUtils::ImportFileAsTexture2D(FilePath);
	Textures.Add(FPaths::GetBaseFilename(FilePath), Texture);
	OnTextureLoadedMulti.Broadcast(Texture, FPaths::GetBaseFilename(FilePath));
}

bool UJJAssetSubsystem::FindTexture(const FString& TexturePath, UTexture2D*& Texture) const
{
	if(!Textures.Contains(TexturePath))
		return false;
	Texture = *Textures.Find(TexturePath);
	
	return IsValid(Texture);
}

TMap<FString, UTexture2D*> UJJAssetSubsystem::GetTextures() const
{
	/*TArray<UTexture2D*> TexturesArr;
	Textures.GenerateValueArray(TexturesArr);*/
	return Textures;
}

USpawnRuntimeMeshActorAsync* USpawnRuntimeMeshActorAsync::SpawnRuntimeMeshActorAsync(const UObject* InWorldContextObj, const UJJModelWrapper* InModel,
		FVector InLocation)
{
	USpawnRuntimeMeshActorAsync* ActorSpawner = NewObject<USpawnRuntimeMeshActorAsync>();
	ActorSpawner->WorldContext = InWorldContextObj;
	ActorSpawner->Model = InModel;
	ActorSpawner->Location = InLocation;
	
	return ActorSpawner;
}

void USpawnRuntimeMeshActorAsync::Activate()
{
	FOnRuntimeActorInitializedStatic OnRuntimeActorInitializedStatic;
	OnRuntimeActorInitializedStatic.BindLambda([this] (const AJJRuntimeMesh* RuntimeActor)
	{
		OnRuntimeActorInitialized.Broadcast(RuntimeActor);
	});
	WorldContext->GetWorld()->GetSubsystem<UJJAssetSubsystem>()->SpawnRuntimeActor(Model->GetWrappedModel(), Location, OnRuntimeActorInitializedStatic);
}

USpawnRuntimeMeshActorFromAssetWrapperAsync* USpawnRuntimeMeshActorFromAssetWrapperAsync::
SpawnRuntimeMeshActorFromAssetWrapperAsync(const UObject* InWorldContextObj, const UJJAssetModelWrapper* AssetWrapper,
		FVector InLocation)
{
	USpawnRuntimeMeshActorFromAssetWrapperAsync* ActorSpawner = NewObject<USpawnRuntimeMeshActorFromAssetWrapperAsync>();
	ActorSpawner->WorldContext = InWorldContextObj;
	ActorSpawner->Model = AssetWrapper;
	ActorSpawner->Location = InLocation;
	
	return ActorSpawner;
}

void USpawnRuntimeMeshActorFromAssetWrapperAsync::Activate()
{
	FOnRuntimeActorInitializedStatic OnRuntimeActorInitializedStatic;
	OnRuntimeActorInitializedStatic.BindLambda([this] (const AJJRuntimeMesh* RuntimeActor)
	{
		OnRuntimeActorInitialized.Broadcast(RuntimeActor);
	});
	WorldContext->GetWorld()->GetSubsystem<UJJAssetSubsystem>()->SpawnRuntimeActor(Model->GetFilePath(), Location, OnRuntimeActorInitializedStatic);
}
