// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "JJAssetModelWrapper.generated.h"

/**
 * 
 */
UCLASS(Blueprintable, BlueprintType)
class JJASSET_API UJJAssetModelWrapper : public UObject
{
	GENERATED_BODY()

public:
	void Init(const FString& InFilePath);

	UPROPERTY(BlueprintReadOnly)
	FString DefaultExtension = ".idasset";

	UFUNCTION(BlueprintPure)
	const FString& GetFilePath() const
	{
		return FilePath;
	}

private:
	FString FilePath;
};
