// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "IDProject.generated.h"

/**
 * 
 */
UCLASS()
class INTERIORDESIGNER_API UIDProject : public UObject
{
	GENERATED_BODY()

public:
	void Init(const FString& Name);

	UFUNCTION(BlueprintPure)
	const FString& GetProjectName() const;

	UFUNCTION(BlueprintPure)
	const FString& GetProjectDir() const;

	UFUNCTION(BlueprintPure)
	FString GetProjectModelDir() const;

	UFUNCTION(BlueprintPure)
	FString GetProjectTextureDir() const;

	UFUNCTION(BlueprintPure)
	FString GetProjectPath() const;

private:
	FString ProjectName;
	FString ProjectDir;
	
};
