// Fill out your copyright notice in the Description page of Project Settings.


#include "Http/IDHttp.h"

#include "JJHttpRequest.h"
#include "Interfaces/IHttpResponse.h"

UFetchFileDescriptions::UFetchFileDescriptions(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer), WorldContextObject(nullptr)
{
}

UFetchFileDescriptions* UFetchFileDescriptions::FetchFileDescriptions(const UObject* WorldContextObject)
{
	UFetchFileDescriptions* BlueprintNode = NewObject<UFetchFileDescriptions>();
	return BlueprintNode;
}

void UFetchFileDescriptions::Activate()
{
	UJJHttpRequest* Request = NewObject<UJJHttpRequest>();
	Request->SetVerb(EJJHttpVerb::GET);
	Request->SetURL("https://localhost:7004/File/MyProjects");

	Request->OnRequestDoneStatic.AddLambda([this] (FHttpResponsePtr Response)
	{
		TSharedRef<TJsonReader<TCHAR>> JsonReader = TJsonReaderFactory<TCHAR>::Create(Response->GetContentAsString());
		TSharedPtr<FJsonObject> JsonObj;
		
		if (FJsonSerializer::Deserialize(JsonReader, JsonObj))
		{
			const TArray<TSharedPtr<FJsonValue>>& JsonArr = JsonObj->GetArrayField("projects");
			TArray<FFile> Files;
			for(TSharedPtr<FJsonValue> FileJson : JsonArr)
			{
				FFile File;
				File.Name = FileJson->AsObject()->GetStringField("name");
				Files.Add(File);
			}
			OnFileDescriptionsFetched.Broadcast(Files);
		}
	});
	
	Request->ProcessRequest();
	
}
