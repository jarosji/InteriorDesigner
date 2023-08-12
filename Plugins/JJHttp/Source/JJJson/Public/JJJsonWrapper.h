// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "JJJsonWrapper.generated.h"

/**
 * 
 */
UCLASS(BlueprintType)
class JJJSON_API UJJJsonWrapper : public UObject
{
	GENERATED_BODY()

public:
	UJJJsonWrapper();
	bool InitializeFromJson(const TSharedPtr<FJsonObject> InJsonObject);

	UFUNCTION(BlueprintCallable, Category="JJJson")
	void SetBoolField(const FString& FieldName, bool InVal);
	
	UFUNCTION(BlueprintCallable, Category="JJJson")
	void SetFloatField(const FString& FieldName, float InVal);

	UFUNCTION(BlueprintCallable, Category="JJJson")
	void SetIntField(const FString& FieldName, int InVal);

	UFUNCTION(BlueprintCallable, Category="JJJson")
	void SetStringField(const FString& FieldName, const FString& InVal);
	
	UFUNCTION(BlueprintCallable, Category="JJJson")
	void SetObjectField(const FString& FieldName, const UJJJsonWrapper* InVal);

	UFUNCTION(BlueprintPure, Category="JJJson")
	bool TryGetBoolField(const FString& FieldName, bool& OutBool)  const; 

	UFUNCTION(BlueprintPure, Category="JJJson")
	bool TryGetFloatField(const FString& FieldName, float& OutVal)  const;
	
	UFUNCTION(BlueprintPure, Category="JJJson")
	bool TryGetIntField(const FString& FieldName, int& OutVal)  const;

	UFUNCTION(BlueprintPure, Category="JJJson")
	bool TryGetStringField(const FString& FieldName, FString& OutVal)  const;

	UFUNCTION(BlueprintPure, Category="JJJson")
	bool TryGetObjectField(const FString& FieldName, UJJJsonWrapper*& OutVal)  const;

	UFUNCTION(BlueprintPure, Category="JJJson")
	bool TryGetArrayObjectField(const FString& FieldName, TArray<UJJJsonWrapper*>& OutVal)  const;

	UFUNCTION(BlueprintCallable, Category="JJJson")
	bool SerializeToString(FString& OutString)  const;

	UFUNCTION(BlueprintCallable, Category="JJJson")
	bool DeserializeFromString(const FString& OutString, UJJJsonWrapper* OutJsonWrapper);

	TSharedPtr<FJsonObject> GetWrappedJsonObject() const;

	
private:
	TSharedPtr<FJsonObject> JsonObject;
};
