// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "JJAssetInterface.generated.h"


struct FJJModel;
UINTERFACE(MinimalAPI, Blueprintable, BlueprintType)
class UJJAssetInterface : public UInterface
{
	GENERATED_BODY()
};

class IJJAssetInterface
{
	GENERATED_BODY()

public:
/*	UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
	FString GetName();*/

private:
	TWeakPtr<FJJModel> Model;
};
