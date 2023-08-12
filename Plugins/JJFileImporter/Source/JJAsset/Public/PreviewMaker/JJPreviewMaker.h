// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "JJPreviewMaker.generated.h"

UCLASS()
class JJASSET_API AJJPreviewMaker : public AActor
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	AJJPreviewMaker();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UTextureRenderTarget2D* RenderTarget;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UMaterialInterface* RTMaterial;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	USceneCaptureComponent2D* CaptureComponent;

public:
	UFUNCTION(BlueprintCallable)
	void StartPreview(AActor* ActorToPreview);

	UFUNCTION(BlueprintCallable)
	void EndPreviewWithSnapshot(AActor* ActorToPreview, const FString& SnaphotPath);
	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

private:
	FVector GetCameraPos(const FVector& BoundsMin, const FVector& BoundsMax, const FVector& Center);
};
