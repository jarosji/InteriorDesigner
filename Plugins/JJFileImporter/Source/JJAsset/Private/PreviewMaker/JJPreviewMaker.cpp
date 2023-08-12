// Fill out your copyright notice in the Description page of Project Settings.


#include "PreviewMaker/JJPreviewMaker.h"

#include "ImageUtils.h"
#include "Components/SceneCaptureComponent2D.h"
#include "Engine/TextureRenderTarget2D.h"
#include "Kismet/KismetRenderingLibrary.h"
#include "Serialization/BufferArchive.h"


// Sets default values
AJJPreviewMaker::AJJPreviewMaker()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	CaptureComponent = CreateDefaultSubobject<USceneCaptureComponent2D>(TEXT("SceneCapture2D"));
	CaptureComponent->SetupAttachment(RootComponent);
}

// Called when the game starts or when spawned
void AJJPreviewMaker::BeginPlay()
{
	Super::BeginPlay();
	
}

void AJJPreviewMaker::StartPreview(AActor* ActorToPreview)
{
	FVector Origin;
	FVector Extent;
	ActorToPreview->GetActorBounds(false, Origin, Extent, true);
	DrawDebugBox(GetWorld(), Origin, Extent, FColor::Purple, true, -1, 0, 10);

	FVector CamPos = GetCameraPos((Origin - Extent), (Origin + Extent), (Origin));
	ActorToPreview->SetActorLocation(GetActorTransform().GetLocation());

	FVector CamCenter = CamPos + GetActorTransform().GetLocation();
	DrawDebugSphere(GetWorld(), CamCenter, 10, 12, FColor(181,0,0), true, -1, 0, 5);

	CaptureComponent->SetWorldLocation(CamCenter);
	CaptureComponent->SetWorldRotation(FRotationMatrix::MakeFromX((ActorToPreview->GetActorTransform().GetLocation() + Origin) - CamCenter).Rotator());
	
	CaptureComponent->ShowOnlyActorComponents(ActorToPreview);
}

void AJJPreviewMaker::EndPreviewWithSnapshot(AActor* ActorToPreview, const FString& SnaphotPath)
{
	FArchive* Ar = IFileManager::Get().CreateFileWriter(*SnaphotPath);

	if (Ar)
	{
		FBufferArchive Buffer;
	
		bool bSuccess = FImageUtils::ExportRenderTarget2DAsPNG(CaptureComponent->TextureTarget, Buffer);

		if (bSuccess)
		{
			Ar->Serialize(const_cast<uint8*>(Buffer.GetData()), Buffer.Num());
		}

		delete Ar;
	}

	//UKismetRenderingLibrary::ExportRenderTarget(GetWorld(), CaptureComponent->TextureTarget, FPaths::GetPath(SnaphotPath), FPaths::GetCleanFilename(SnaphotPath));

	ActorToPreview->Destroy();
}

// Called every frame
void AJJPreviewMaker::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

FVector AJJPreviewMaker::GetCameraPos(const FVector& BoundsMin, const FVector& BoundsMax, const FVector& Center)
{
	const FVector& A = BoundsMax - BoundsMin;
	float Max = FMath::Max3(A.X, A.Y, A.Z);
	float Deg = FMath::Tan(FMath::DegreesToRadians(90) * 0.5f) * 2;

	float Cam = ((Max / Deg) * 1) + (Max * 0.5f);
	FVector Final = Cam * GetActorForwardVector();

	return Center - Final;
}

