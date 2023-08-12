// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "HttpModule.h"
#include "JJHttpRequest.generated.h"

/*class IHttpRequest;
class IHttpResponse;

typedef TSharedPtr<IHttpRequest, ESPMode::ThreadSafe> FHttpRequestPtr;
typedef  TSharedPtr<IHttpResponse, ESPMode::ThreadSafe> FHttpResponsePtr;*/

UENUM(BlueprintType)
enum class EJJHttpVerb : uint8
{
	GET,
	POST,
	PUT,
	DELETE
};

UENUM(BlueprintType)
enum class EJJHttpContentType : uint8
{
	multipart_form_data			UMETA(DisplayName = "multipart/form-data"),
	application_octet_stream	UMETA(DisplayName = "application/octet-stream"),
	application_json			UMETA(DisplayName = "application/json")
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnRequestDone, bool, bIsSuccessful);
DECLARE_MULTICAST_DELEGATE_OneParam(FOnRequestDoneStatic, FHttpResponsePtr Json);

UCLASS(Blueprintable, BlueprintType)
class JJHTTP_API UJJHttpRequest : public UObject
{
	GENERATED_BODY()

public:
	UJJHttpRequest();
	
	UPROPERTY(BlueprintAssignable)
	FOnRequestDone OnRequestDone;

	FOnRequestDoneStatic OnRequestDoneStatic;

public:
	
	UFUNCTION(BlueprintCallable, Category = "JJHttp")
	void SetVerb(EJJHttpVerb Verb);

	UFUNCTION(BlueprintCallable, Category = "JJHttp")
	void SetContentType(EJJHttpContentType ContentType);

	UFUNCTION(BlueprintCallable, Category = "JJHttp")
	void SetHeader(const FString& HeaderName, const FString& HeaderValue);

	UFUNCTION(BlueprintCallable, Category = "JJHttp")
	void AppendToHeader(const FString& HeaderName, const FString& HeaderValue);

	UFUNCTION(BlueprintCallable, Category = "JJHttp")
	void SetURL(const FString& Url);

	//UFUNCTION(BlueprintCallable, Category = "JJHttp")
	void SetJsonContent(const TSharedPtr<FJsonObject> JsonContent);

	UFUNCTION(BlueprintCallable, Category = "JJHttp")
	void SetBinaryContent(const TArray<uint8>& BinaryData);

	UFUNCTION(BlueprintCallable, Category = "JJHttp")
	void SetBinaryContentFromStorage(const FString& FilePath);

	UFUNCTION(BlueprintCallable, Category = "JJHttp")
	void ProcessRequest();

private:
	FHttpRequestPtr Http;
};
