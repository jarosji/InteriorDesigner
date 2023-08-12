// Fill out your copyright notice in the Description page of Project Settings.


#include "JJHttpRequest.h"
#include "HttpModule.h"

UJJHttpRequest::UJJHttpRequest()
{
	Http = FHttpModule::Get().CreateRequest();
}

void UJJHttpRequest::SetVerb(EJJHttpVerb Verb)
{
	switch (Verb)
	{
		case EJJHttpVerb::GET: Http->SetVerb("GET"); break;
		case EJJHttpVerb::POST: Http->SetVerb("POST"); break;
		case EJJHttpVerb::PUT: Http->SetVerb("PUT"); break;
		case EJJHttpVerb::DELETE: Http->SetVerb("DELETE"); break;
	}
}

void UJJHttpRequest::SetContentType(EJJHttpContentType ContentType)
{
	switch (ContentType)
	{
		case EJJHttpContentType::application_json: Http->SetHeader("Content-Type", "application/json"); break;
		case EJJHttpContentType::application_octet_stream: Http->SetHeader("Content-Type", "application/octet-stream"); break;
		case EJJHttpContentType::multipart_form_data: Http->SetHeader("Content-Type", "multipart/form-data; boundary=bodyBoundary"); break;
	}
}

void UJJHttpRequest::SetHeader(const FString& HeaderName, const FString& HeaderValue)
{
	Http->SetHeader(HeaderName, HeaderValue);
}

void UJJHttpRequest::AppendToHeader(const FString& HeaderName, const FString& HeaderValue)
{
	Http->AppendToHeader(HeaderName, HeaderValue);
}

void UJJHttpRequest::SetURL(const FString& Url)
{
	Http->SetURL(Url);
}

void UJJHttpRequest::SetJsonContent(const TSharedPtr<FJsonObject> JsonContent)
{
	FString OutputString;
	TSharedRef< TJsonWriter<> > Writer = TJsonWriterFactory<TCHAR>::Create(&OutputString);
	FJsonSerializer::Serialize(JsonContent.ToSharedRef(), Writer);
	Http->SetContentAsString(OutputString);
}

void UJJHttpRequest::SetBinaryContent(const TArray<uint8>& BinaryData)
{
	Http->SetContent(BinaryData);
}

void UJJHttpRequest::SetBinaryContentFromStorage(const FString& FilePath)
{;
	TArray<uint8> FileData;
	const bool FileLoaded = FFileHelper::LoadFileToArray(FileData, *FilePath);
	const FString& FileName = FPaths::GetCleanFilename(FilePath);
	
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
	
	Http->SetContent(data);
}

void UJJHttpRequest::ProcessRequest()
{
	Http->OnProcessRequestComplete().BindLambda([this] (FHttpRequestPtr Request, FHttpResponsePtr Response, bool bWasSuccessful) {
		OnRequestDone.Broadcast(bWasSuccessful);
		OnRequestDoneStatic.Broadcast(Response);
	});
	
	Http->OnRequestProgress().BindLambda([this] (FHttpRequestPtr Request, int32 BytesSent, int32 BytesReceived) {
		
	});
	Http->ProcessRequest();
}
