// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "JJFileDialogLibrary.generated.h"

/**
 * 
 */
UCLASS()
class JJCORE_API UJJFileDialogLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

private:
	static constexpr int MAX_PATH_SIZE = 260;

public:
	UFUNCTION(BlueprintCallable, Category = "JJFileDialog")
	static bool OpenFileDialog(FString& FilePath, const FString& DialogTitle, const FString& DefaultPath,
		const TArray<FString>& Extensions, const TArray<FString>& Descriptions);

private:
	static std::string FormatFilter(const TArray<FString>& Extensions, const TArray<FString>& Descriptions);
};
