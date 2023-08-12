// Fill out your copyright notice in the Description page of Project Settings.


#include "JJHttpSubsystem.h"

#include "ImageUtils.h"
#include "JJHttpRequest.h"
#include "JJJsonWrapper.h"
#include "Interfaces/IHttpResponse.h"
/*#include "HttpModule.h"



void UJJHttpSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);
}

void UJJHttpSubsystem::Deinitialize()
{
	Super::Deinitialize();
}

void UJJHttpSubsystem::CallUrl(FString Url)
{
	Http = &FHttpModule::Get();
	FHttpRequestPtr Request = Http->CreateRequest();
	
	Request->SetURL(Url);
	Request->SetVerb("GET");

	Request->OnProcessRequestComplete().BindUObject(this, &UJJHttpSubsystem::CompletedHTTPRequest);
	Request->OnRequestProgress().BindUObject(this, &UJJHttpSubsystem::ProgressHTTPRequest);

	Request->ProcessRequest();
}

void UJJHttpSubsystem::UploadFile(FString FilePath, FString Url, const TMap<FString, FString>& Headers)
{
	Http = &FHttpModule::Get();
	TArray<uint8> FileData;
	const bool FileLoaded = FFileHelper::LoadFileToArray(FileData, *FilePath);

	const FString& FileName = FPaths::GetCleanFilename(FilePath);
	FHttpRequestPtr Request = Http->CreateRequest();
	
	Request->SetURL(Url);
	Request->SetVerb("POST");

	//Default Headers
	Request->SetHeader("Content-Type", TEXT("multipart/form-data; boundary=bodyBoundary"));

	//Set Additional Headers
	TArray<FString> HeaderNames;
	Headers.GetKeys(HeaderNames);
	for(FString HeaderName : HeaderNames)
	{
		Request->SetHeader(HeaderName, *Headers.Find(HeaderName));
	}

	FString a = "\r\n--bodyBoundary\r\n";
	FString b = "Content-Disposition: form-data; name=\"file\";  filename=\"" + FileName + "\"\r\n";
	FString c = "Content-Type: application/octet-stream\r\n\r\n";
	FString e = "\r\n--bodyBoundary--\r\n";

	TArray<uint8> data;
	data.Append((uint8*) TCHAR_TO_UTF8(*a), a.Len());
	data.Append((uint8*) TCHAR_TO_UTF8(*b), b.Len());
	data.Append((uint8*) TCHAR_TO_UTF8(*c), c.Len());
	data.Append(FileData);
	data.Append((uint8*) TCHAR_TO_UTF8(*e), e.Len());

	Request->SetContent(data);

	Request->OnProcessRequestComplete().BindUObject(this, &UJJHttpSubsystem::CompletedHTTPRequest);
	Request->OnRequestProgress().BindUObject(this, &UJJHttpSubsystem::ProgressHTTPRequest);

	Request->ProcessRequest();
}

void UJJHttpSubsystem::DownloadFile(FString FilePath, FString Url, FString FileName)
{
	Http = &FHttpModule::Get();
	FHttpRequestPtr Request = Http->CreateRequest();
	
	Request->SetURL(Url);
	Request->SetVerb("GET");

	Request->OnProcessRequestComplete().BindUObject(this, &UJJHttpSubsystem::CompletedHTTPRequest);
	Request->OnRequestProgress().BindUObject(this, &UJJHttpSubsystem::ProgressHTTPRequest);

	Request->ProcessRequest();
}

void UJJHttpSubsystem::CompletedHTTPRequest(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bWasSuccessful)
{
	if(!bWasSuccessful)
		return;
	
	FString a = "D:/Test.zip";
	FFileHelper::SaveArrayToFile(Response->GetContent(), *a);
	
	TSharedRef<TJsonReader<TCHAR>> JsonReader = TJsonReaderFactory<TCHAR>::Create(Response->GetContentAsString());
	TArray<TSharedPtr<FJsonValue>> OutArray;

	TArray<FFile> Files;
	if (FJsonSerializer::Deserialize(JsonReader, OutArray))
	{
		for(auto Object : OutArray)
		{
			FFile File;
			File.Name = Object->AsObject()->GetStringField("name");
			Files.Add(File);
		}
	}
	
	OnRequestDone.Broadcast(Files);
}

void UJJHttpSubsystem::ProgressHTTPRequest(FHttpRequestPtr Request, int32 BytesSent, int32 BytesReceived)
{
}*/
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
		Request->SetJsonContent(JsonWrapperContent->GetWrappedJsonObject());

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
	TSharedPtr<FJsonObject> JsonObj = MakeShared<FJsonObject>();
	JsonObj->SetStringField("userName", Username);
	JsonObj->SetStringField("password", Password);
	
	UJJHttpRequest* Request = NewObject<UJJHttpRequest>();
	Request->SetURL(AuthUrl);
	Request->SetVerb(EJJHttpVerb::POST);
	Request->SetContentType(EJJHttpContentType::application_json);
	Request->SetJsonContent(JsonObj);

	Request->OnRequestDoneStatic.AddLambda([this, OnLoginSuccessful, OnLoginFailed] (FHttpResponsePtr Response)
	{
		const FString& Token = Response->GetContentAsString();
		if(Token.Len() > 0)
		{
			SetBearerToken(Token);

			FUserInfo UserInfo;
			OnLoginSuccessful.ExecuteIfBound(UserInfo, Token);
			OnLoginMulti.Broadcast(true);
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
