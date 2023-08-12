// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "JJStorageLibrary.generated.h"

UCLASS()
class JJCORE_API UJJStorageLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:

	UFUNCTION(BlueprintCallable, Category="JJStorageHelpers")
	static bool CopyFile(const FString& SrcPath, const FString& DstPath);
};
