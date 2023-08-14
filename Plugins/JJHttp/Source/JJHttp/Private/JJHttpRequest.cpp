// Fill out your copyright notice in the Description page of Project Settings.


#include "JJHttpRequest.h"
#include "JJJsonWrapper.h"
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

void UJJHttpRequest::SetJsonContent(const UJJJsonWrapper* JsonContent)
{
	FString OutputString;
	if(JsonContent->SerializeToString(OutputString))
	{
		Http->SetContentAsString(OutputString);
	}
}

void UJJHttpRequest::SetBinaryContent(const TArray<uint8>& BinaryData)
{
	Http->SetContent(BinaryData);
}

void UJJHttpRequest::SetBinaryContentFromStorage(const FString& FilePath)
{
	TArray<uint8> FileData;
	if(FFileHelper::LoadFileToArray(FileData, *FilePath))
	{
		const FString& FileName = FPaths::GetCleanFilename(FilePath);
	
		FString A = "\r\n--bodyBoundary\r\n";
		FString B = "Content-Disposition: form-data; name=\"file\";  filename=\"" + FileName + "\"\r\n";
		FString C = "Content-Type: application/octet-stream\r\n\r\n";
		FString D = "\r\n--bodyBoundary--\r\n";

		TArray<uint8> Data;
		Data.Append((uint8*) TCHAR_TO_UTF8(*A), A.Len());
		Data.Append((uint8*) TCHAR_TO_UTF8(*B), B.Len());
		Data.Append((uint8*) TCHAR_TO_UTF8(*C), C.Len());
		Data.Append(FileData);
		Data.Append((uint8*) TCHAR_TO_UTF8(*D), D.Len());
	
		Http->SetContent(Data);
	}
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
