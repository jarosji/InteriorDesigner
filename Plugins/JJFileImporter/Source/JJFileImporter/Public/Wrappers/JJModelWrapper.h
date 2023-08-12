// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "ImportedFiles/JJModel.h"
#include "JJModelWrapper.generated.h"

/**
 * Lightweight wrapper for Imported models with automatic clean-up after Garbage-collect Tick
 */
UCLASS(BlueprintType, Blueprintable)
class JJFILEIMPORTER_API UJJModelWrapper : public UObject
{
	GENERATED_BODY()
	
public:
	void Init(TSharedPtr<FJJModel> InModel);

	UFUNCTION(BlueprintCallable)
	void SetImportSettings(const FJJImportSettings& ImportSettings);
	
	UFUNCTION(BlueprintPure)
	TArray<FVector>& GetPositions(int MeshSection) const;

	UFUNCTION(BlueprintPure)
	TArray<int32>& GetIndices(int MeshSection) const;

	UFUNCTION(BlueprintPure)
	TArray<FVector>& GetNormals(int MeshSection) const;

	UFUNCTION(BlueprintPure)
	TArray<FVector2D>& GetTexCoords(int MeshSection) const;

	UFUNCTION(BlueprintPure)
	FJJMaterial& GetJJMaterial(int MeshSection) const;

	UFUNCTION(BlueprintPure)
	int GetNumberOfSections() const;

	UFUNCTION(BlueprintPure)
	FString GetFilePath() const;

	UFUNCTION(BlueprintPure)
	FString GetFileName() const;

	TSharedPtr<FJJModel> GetWrappedModel() const;

private:
	TSharedPtr<FJJModel> WrappedModel;
};
