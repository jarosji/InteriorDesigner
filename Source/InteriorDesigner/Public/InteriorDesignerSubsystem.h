// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "InteriorDesignerSubsystem.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnSelectionChanged, AActor*, SelectedActor);

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnProjectChanged, UIDProject*, NewProject);

class AJJRuntimeMesh;
class AJJRoomV2;


UCLASS()
class INTERIORDESIGNER_API UInteriorDesignerSubsystem : public UWorldSubsystem
{
	GENERATED_BODY()

public:
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;
	virtual void Deinitialize() override;

#pragma region Project
public:

	UPROPERTY(BlueprintAssignable)
	FOnProjectChanged OnProjectChanged;
	
	UFUNCTION(BlueprintCallable)
	void CreateProject(const FString& ProjectName);

	UFUNCTION(BlueprintCallable)
	void SaveProject(const UIDProject* ProjectToSave);

	UFUNCTION(BlueprintCallable)
	UIDProject* LoadProject(const FString& FilePath);

	UFUNCTION(BlueprintCallable)
	void CloseProject();

	UFUNCTION(BlueprintPure)
	UIDProject* GetCurrentProject() const;

#pragma endregion 

	//UPROPERTY(BlueprintReadWrite)
	//TArray<AJJRoomDynamicActor*> Rooms;

	UPROPERTY(BlueprintAssignable)
	FOnSelectionChanged OnSelectionChanged;

	UFUNCTION(BlueprintPure)
	AActor* GetSelectedActor() const
	{
		return SelectedActor;
	}

	UFUNCTION(BlueprintCallable)
	void SetSelection(AActor* NewSelection)
	{
		if(IsValid(SelectedActor) && NewSelection != SelectedActor)
			Cast<UStaticMeshComponent>(SelectedActor->GetComponentByClass(UStaticMeshComponent::StaticClass()))->SetRenderCustomDepth(false);
		SelectedActor = NewSelection;
		Cast<UStaticMeshComponent>(SelectedActor->GetComponentByClass(UStaticMeshComponent::StaticClass()))->SetRenderCustomDepth(true);
		OnSelectionChanged.Broadcast(NewSelection);
	}

	UFUNCTION(BlueprintCallable)
	void EmptySelection()
	{
		if(IsValid(SelectedActor))
			Cast<UStaticMeshComponent>(SelectedActor->GetComponentByClass(UStaticMeshComponent::StaticClass()))->SetRenderCustomDepth(false);
		SelectedActor = nullptr;
		OnSelectionChanged.Broadcast(nullptr);
	}

	UFUNCTION(BlueprintCallable)
	void AddRuntimeMeshActor(AJJRuntimeMesh* ActorToAdd);

	UFUNCTION(BlueprintCallable)
	void AddRoomActor(AJJRoomV2* ActorToAdd);

	UFUNCTION(BlueprintPure)
	TArray<AJJRuntimeMesh*>& GetRuntimeMeshActors();

	UFUNCTION(BlueprintPure)
	TArray<AJJRoomV2*>& GetRoomActors();

private:
	UPROPERTY() TObjectPtr<UIDProject> CurrentProject;
    UPROPERTY() TObjectPtr<AActor> SelectedActor;

	UPROPERTY() TArray<TObjectPtr<AJJRuntimeMesh>> RuntimeMeshes;
	UPROPERTY() TArray<TObjectPtr<AJJRoomV2>> Rooms;

	friend class UIDSave;
};
