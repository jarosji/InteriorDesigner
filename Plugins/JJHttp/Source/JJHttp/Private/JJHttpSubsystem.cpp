// Fill out your copyright notice in the Description page of Project Settings.


#include "JJHttpSubsystem.h"

#include "JJHttpRequest.h"
#include "JJJsonWrapper.h"
#include "Interfaces/IHttpResponse.h"

void UJJHttpSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);
}

void UJJHttpSubsystem::Deinitialize()
{
	Super::Deinitialize();
}

bool UJJHttpSubsystem::IsLoggedIn()
{
	FHttpModule* Http = &FHttpModule::Get();
	if(Http->GetDefaultHeaders().Contains("Authorization") == false)
		return false;

	//Maybe check for Token expiration? (Try to refresh on fail?)

	return Http->GetDefaultHeaders().Find("Authorization")->Len() > 0;
}

void UJJHttpSubsystem::CallUrl(const FString& Url, EJJHttpVerb Verb, UJJJsonWrapper* JsonWrapperContent,
	FOnUrlCallSuccessful OnUrlCallSuccessful, FOnUrlCallFailed OnUrlCallFailed)
{
	UJJHttpRequest* Request = NewObject<UJJHttpRequest>();
	Request->SetURL(Url);
	Request->SetVerb(Verb);
	Request->SetContentType(EJJHttpContentType::application_json);
	if(IsValid(JsonWrapperContent))
		Request->SetJsonContent(JsonWrapperContent);

	Request->OnRequestDoneStatic.AddLambda([this, OnUrlCallFailed, OnUrlCallSuccessful] (FHttpResponsePtr Response)
	{
		TSharedRef<TJsonReader<TCHAR>> JsonReader = TJsonReaderFactory<TCHAR>::Create(Response->GetContentAsString());
		TSharedPtr<FJsonObject> JsonObj;
		
		if (FJsonSerializer::Deserialize(JsonReader, JsonObj))
		{
			UJJJsonWrapper* JsonWrapper = NewObject<UJJJsonWrapper>();
			JsonWrapper->InitializeFromJson(JsonObj);
			OnUrlCallSuccessful.ExecuteIfBound(JsonWrapper, Response->GetResponseCode());
		}
		else
		{
			OnUrlCallFailed.ExecuteIfBound(Response->GetResponseCode());
		}
	});
	
	Request->ProcessRequest();
}

void UJJHttpSubsystem::Login(const FString& AuthUrl, const FString& Username, const FString& Password,
                             FOnLoginSuccessful OnLoginSuccessful, FOnLoginFailed OnLoginFailed)
{
	UJJJsonWrapper* JsonObj = NewObject<UJJJsonWrapper>();
	JsonObj->SetStringField("userName", Username);
	JsonObj->SetStringField("password", Password);
	
	UJJHttpRequest* Request = NewObject<UJJHttpRequest>();
	Request->SetURL(AuthUrl);
	Request->SetVerb(EJJHttpVerb::POST);
	Request->SetContentType(EJJHttpContentType::application_json);
	Request->SetJsonContent(JsonObj);

	Request->OnRequestDoneStatic.AddLambda([this, OnLoginSuccessful, OnLoginFailed] (FHttpResponsePtr Response)
	{
		bool Error = false;
		const FString& Content = Response->GetContentAsString();
		if(Content.Len() > 0)
		{
			UJJJsonWrapper* Json = NewObject<UJJJsonWrapper>();
			if(Json->DeserializeFromString(Content))
			{
				UJJJsonWrapper* OutVal = NewObject<UJJJsonWrapper>();
				if(Json->TryGetObjectField("errors", OutVal))
				{
					OnLoginFailed.ExecuteIfBound(Response->GetResponseCode());
					Error = true;
				}
			}

			if(Error == false)
			{
				SetBearerToken(Content);

				FUserInfo UserInfo;
				OnLoginSuccessful.ExecuteIfBound(UserInfo, Content);
				OnLoginMulti.Broadcast(true);
			}
		}
		else
		{
			OnLoginFailed.ExecuteIfBound(Response->GetResponseCode());
		}
	});
	
	Request->ProcessRequest();
}

void UJJHttpSubsystem::Logout()
{
	EmptyBearerToken();
	OnLoginMulti.Broadcast(false);
}

void UJJHttpSubsystem::UploadFile(const FString& Url, const FString& FilePath)
{
	UJJHttpRequest* Request = NewObject<UJJHttpRequest>();
	Request->SetURL(Url);
	Request->SetVerb(EJJHttpVerb::POST);
	Request->SetContentType(EJJHttpContentType::multipart_form_data);
	Request->SetBinaryContentFromStorage(FilePath);
	Request->ProcessRequest();
}

void UJJHttpSubsystem::DownloadFile(const FString& Url, const FString& FilePath, const FString& FileNameOnServer, FOnDownloadFinished OnDownloadFinished)
{
	UJJHttpRequest* Request = NewObject<UJJHttpRequest>();
	Request->SetURL(Url + "?name=" + FileNameOnServer);
	Request->SetVerb(EJJHttpVerb::GET);
	Request->OnRequestDoneStatic.AddLambda([this, FilePath, OnDownloadFinished] (FHttpResponsePtr Response)
	{
		FFileHelper::SaveArrayToFile(Response->GetContent(), *FilePath);
		OnDownloadFinished.Execute(true);
	});

	Request->ProcessRequest();
}

void UJJHttpSubsystem::SetBearerToken(const FString& Token)
{
	FHttpModule* Http = &FHttpModule::Get();
	Http->AddDefaultHeader("Authorization", "Bearer " + Token);
}

void UJJHttpSubsystem::EmptyBearerToken()
{
	FHttpModule* Http = &FHttpModule::Get();
	Http->AddDefaultHeader("Authorization", "");
}
