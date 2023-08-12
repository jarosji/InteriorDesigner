// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "JJHttpSubsystem.generated.h"

USTRUCT(BlueprintType)
struct FUserInfo
{
	GENERATED_BODY()

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	FString Username;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	FString Email;
};

//DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnRequestDone, const TArray<FFile>&, Files);
DECLARE_DYNAMIC_DELEGATE_OneParam(FOnDownloadFinished, bool, bIsSuccessful);

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnLoginMulti, bool, bIsLoggedIn);
DECLARE_DYNAMIC_DELEGATE_TwoParams(FOnLoginSuccessful, FUserInfo, UserInfo, FString, BearerToken);
DECLARE_DYNAMIC_DELEGATE_OneParam(FOnLoginFailed, int, StatucCode);

DECLARE_DYNAMIC_DELEGATE_TwoParams(FOnUrlCallSuccessful, UJJJsonWrapper*, JsonWrapper, int, StatucCode);
DECLARE_DYNAMIC_DELEGATE_OneParam(FOnUrlCallFailed, int, StatucCode);

class FHttpModule;
class FJsonObject;
class UJJJsonWrapper;

UCLASS()
class JJHTTP_API UJJHttpSubsystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()

public:
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;
	virtual void Deinitialize() override;

	UPROPERTY(BlueprintAssignable)
	FOnLoginMulti OnLoginMulti;

	UFUNCTION(BlueprintCallable)
	bool IsLoggedIn();

	UFUNCTION(BlueprintCallable)
	void CallUrl(const FString& Url, EJJHttpVerb Verb, 
		UJJJsonWrapper* JsonWrapperContent,
		FOnUrlCallSuccessful OnUrlCallSuccessful, FOnUrlCallFailed OnUrlCallFailed);

	UFUNCTION(BlueprintCallable)
    void Login(const FString& AuthUrl, const FString& Username, const FString& Password,
    	FOnLoginSuccessful OnLoginSuccessful, FOnLoginFailed OnLoginFailed);

	UFUNCTION(BlueprintCallable)
	void Logout();

	UFUNCTION(BlueprintCallable)
	void UploadFile(const FString& Url, const FString& FilePath);

	UFUNCTION(BlueprintCallable)
	void DownloadFile(const FString& Url, const FString& FilePath, const FString& FileNameOnServer, FOnDownloadFinished OnDownloadFinished);

	UFUNCTION(BlueprintCallable)
	void SetBearerToken(const FString& Token);

	UFUNCTION(BlueprintCallable)
	void EmptyBearerToken();

/*public:
	UPROPERTY(BlueprintAssignable)
	FOnRequestDone OnRequestDone;

private:
	FHttpModule* Http;

	void CompletedHTTPRequest(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bWasSuccessful);
	void ProgressHTTPRequest(FHttpRequestPtr Request, int32 BytesSent, int32 BytesReceived);*/
};
