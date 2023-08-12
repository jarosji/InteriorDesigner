// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Kismet/BlueprintAsyncActionBase.h"
#include "IDHttp.generated.h"

USTRUCT(BlueprintType)
struct FFile
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadWrite)
	FString Name;
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnFileDescriptionsFetched, const TArray<FFile>&, FileDesc);

UCLASS()
class INTERIORDESIGNER_API UFetchFileDescriptions : public UBlueprintAsyncActionBase
{
	GENERATED_UCLASS_BODY()
	
public:
	
	UPROPERTY(BlueprintAssignable)
	FOnFileDescriptionsFetched OnFileDescriptionsFetched;
	
	UFUNCTION(BlueprintCallable, meta = (BlueprintInternalUseOnly = "true", WorldContext = "WorldContextObject"), Category = "IDHttp")
	static UFetchFileDescriptions* FetchFileDescriptions(const UObject* WorldContextObject);
	
	virtual void Activate() override;

private:
	const UObject* WorldContextObject;
};
