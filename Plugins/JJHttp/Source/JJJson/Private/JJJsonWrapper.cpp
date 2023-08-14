// Fill out your copyright notice in the Description page of Project Settings.


#include "JJJsonWrapper.h"

UJJJsonWrapper::UJJJsonWrapper()
{
	JsonObject = MakeShared<FJsonObject>();
}

bool UJJJsonWrapper::InitializeFromJson(const TSharedPtr<FJsonObject> InJsonObject)
{
	if(!InJsonObject.IsValid())
		return false;

	JsonObject.Reset();
	JsonObject = InJsonObject;
	return true;
}

void UJJJsonWrapper::SetBoolField(const FString& FieldName, bool InVal)
{
	JsonObject->SetBoolField(FieldName, InVal);
}

void UJJJsonWrapper::SetFloatField(const FString& FieldName, float InVal)
{
	JsonObject->SetNumberField(FieldName, InVal);
}

void UJJJsonWrapper::SetIntField(const FString& FieldName, int InVal)
{
	JsonObject->SetNumberField(FieldName, InVal);
}

void UJJJsonWrapper::SetStringField(const FString& FieldName, const FString& InVal)
{
	JsonObject->SetStringField(FieldName, InVal);
}

void UJJJsonWrapper::SetObjectField(const FString& FieldName, const UJJJsonWrapper* InVal)
{
	JsonObject->SetObjectField(FieldName, InVal->GetWrappedJsonObject());
}

bool UJJJsonWrapper::TryGetBoolField(const FString& FieldName, bool& OutVal) const
{
	return JsonObject->TryGetBoolField(FieldName, OutVal);
}

bool UJJJsonWrapper::TryGetFloatField(const FString& FieldName, float& OutVal) const
{
	return JsonObject->TryGetNumberField(FieldName, OutVal);
}

bool UJJJsonWrapper::TryGetIntField(const FString& FieldName, int& OutVal) const
{
	return JsonObject->TryGetNumberField(FieldName, OutVal);
}

bool UJJJsonWrapper::TryGetStringField(const FString& FieldName, FString& OutVal) const
{
	return JsonObject->TryGetStringField(FieldName, OutVal);
}

bool UJJJsonWrapper::TryGetObjectField(const FString& FieldName, UJJJsonWrapper*& OutVal) const
{
	const TSharedPtr<FJsonObject>* OutJsonObject;
	if(JsonObject->TryGetObjectField(FieldName, OutJsonObject))
	{
		OutVal->InitializeFromJson(*OutJsonObject);
		return true;
	}

	return false;
}

bool UJJJsonWrapper::TryGetArrayObjectField(const FString& FieldName, TArray<UJJJsonWrapper*>& OutVal) const
{
	 const TArray<TSharedPtr<FJsonValue>>* OutJsonObject;
	 if(JsonObject->TryGetArrayField(FieldName, OutJsonObject))
     {
     	for(const TSharedPtr<FJsonValue> Val : *OutJsonObject)
     	{
     		auto New = NewObject<UJJJsonWrapper>();
    		New->InitializeFromJson(Val->AsObject());
    		OutVal.Add(New);
    	}
 
	 	return true;
     }

	return false;
}

bool UJJJsonWrapper::SerializeToString(FString& OutString) const
{
	TSharedRef<TJsonWriter<>> Writer = TJsonWriterFactory<TCHAR>::Create(&OutString);
	return FJsonSerializer::Serialize(JsonObject.ToSharedRef(), Writer);
}

bool UJJJsonWrapper::DeserializeFromString(const FString& OutString)
{
	TSharedRef<TJsonReader<>> Reader = TJsonReaderFactory<TCHAR>::Create(OutString);
	return FJsonSerializer::Deserialize(Reader, JsonObject);
}

TSharedPtr<FJsonObject> UJJJsonWrapper::GetWrappedJsonObject() const
{
	return JsonObject;
}
