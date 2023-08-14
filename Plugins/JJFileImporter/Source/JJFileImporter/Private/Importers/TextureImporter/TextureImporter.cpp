#include "JJTextureImporter.h"
#include "ImageUtils.h"

EPixelFormat JJTextureImporter::RawImageFormatToPixelFormat(ERawImageFormat::Type RawImageFormat)
{
	switch (RawImageFormat)
	{
	case ERawImageFormat::G8:			return EPixelFormat::PF_G8;
		case ERawImageFormat::G16:		return EPixelFormat::PF_G16;
		case ERawImageFormat::R16F:		return EPixelFormat::PF_R16F;
		case ERawImageFormat::R32F:		return EPixelFormat::PF_R32_FLOAT;
		case ERawImageFormat::BGRA8:	return EPixelFormat::PF_B8G8R8A8;
		case ERawImageFormat::BGRE8:	return EPixelFormat::PF_B8G8R8A8;
		case ERawImageFormat::RGBA16:	return EPixelFormat::PF_R16G16B16A16_SINT;
		case ERawImageFormat::RGBA16F:	return EPixelFormat::PF_FloatRGBA;
		case ERawImageFormat::RGBA32F:	return EPixelFormat::PF_A32B32G32R32F;
		default: return PF_Unknown;
	}
}

JJTextureImporter::FTexture2DResource::FTexture2DResource(UTexture2D* InOwner, FTextureRHIRef InRHITextureRef)
	:Owner(InOwner), SizeX(InRHITextureRef->GetSizeXYZ().X), SizeY(InRHITextureRef->GetSizeXYZ().Y)
{
	TextureRHI = InRHITextureRef;
	bSRGB = (TextureRHI->GetFlags() & TexCreate_SRGB) != TexCreate_None;
	bIgnoreGammaConversions = !bSRGB;
	bGreyScaleFormat = (TextureRHI->GetFormat() == PF_G8) || (TextureRHI->GetFormat() == PF_BC4);
}

JJTextureImporter::FTexture2DResource::~FTexture2DResource()
{
	if (IsValid(Owner))
	{
		Owner->SetResource(nullptr);
	}
}

void JJTextureImporter::FTexture2DResource::InitRHI()
{
	// Create the sampler state RHI resource.
	FSamplerStateInitializerRHI SamplerStateInitializer(SF_Trilinear);
	SamplerStateRHI = GetOrCreateSamplerState(SamplerStateInitializer);

	// Create a custom sampler state for using this texture in a deferred pass, where ddx / ddy are discontinuous
	FSamplerStateInitializerRHI DeferredPassSamplerStateInitializer(
		SF_Trilinear,
		AM_Wrap,
		AM_Wrap,
		AM_Wrap,
		0,
		// Disable anisotropic filtering, since aniso doesn't respect MaxLOD
		1,
		0,
		// Prevent the less detailed mip levels from being used, which hides artifacts on silhouettes due to ddx / ddy being very large
		// This has the side effect that it increases minification aliasing on light functions
		2
	);

	DeferredPassSamplerStateRHI = GetOrCreateSamplerState(DeferredPassSamplerStateInitializer);
}

void JJTextureImporter::FTexture2DResource::ReleaseRHI()
{
	if (IsValid(Owner))
	{
		RHIUpdateTextureReference(Owner->TextureReference.TextureReferenceRHI, nullptr);
	}
	FTextureResource::ReleaseRHI();
}

void JJTextureImporter::ImportTexture(const FString& BaseFilename, UTexture2D*& NewTexture)
{
	FImage OutImage;
	if(!FImageUtils::LoadImage(*BaseFilename, OutImage))
	{
		NewTexture = nullptr;
		return;
	}

	const int32 NumMips = 1;
	const int32 TextureSizeX = OutImage.SizeX;
	const int32 TextureSizeY = OutImage.SizeY;
	EPixelFormat PixelFormat = RawImageFormatToPixelFormat(OutImage.Format);

	FTextureDataResource TextureData(OutImage.RawData.GetData(), OutImage.RawData.Num());
	FRHIResourceCreateInfo CreateInfo(TEXT("RuntimeImageReaderTextureData"));
	CreateInfo.BulkData = &TextureData;
	
	FGraphEventRef CreateTextureTask = FFunctionGraphTask::CreateAndDispatchWhenReady(
	   [this, TextureSizeX, TextureSizeY, PixelFormat, NumMips, CreateInfo]()
	   {
			RHITexture2D = RHICreateTexture(
				FRHITextureCreateDesc::Create2D(CreateInfo.DebugName)
				.SetExtent(TextureSizeX, TextureSizeY)
				.SetFormat(PixelFormat)
				.SetNumMips(NumMips)
				.SetNumSamples(1)
				.SetFlags(TexCreate_ShaderResource | TexCreate_SRGB)
				.SetInitialState(ERHIAccess::Unknown)
				.SetExtData(CreateInfo.ExtData)
				.SetBulkData(CreateInfo.BulkData)
				.SetGPUMask(CreateInfo.GPUMask)
				.SetClearValue(CreateInfo.ClearValueBinding)
			);
	   }, TStatId(), nullptr, ENamedThreads::ActualRenderingThread
   );
	CreateTextureTask->Wait();
	
	FTextureResource* NewTextureResource = new FTexture2DResource(NewTexture, RHITexture2D);
	NewTexture->SetResource(NewTextureResource);

	FGraphEventRef UpdateResourceTask = FFunctionGraphTask::CreateAndDispatchWhenReady(
		[NewTexture, this, &NewTextureResource]()
		{
			NewTextureResource->InitResource();
			RHIUpdateTextureReference(NewTexture->TextureReference.TextureReferenceRHI, RHITexture2D);
			NewTextureResource->SetTextureReference(NewTexture->TextureReference.TextureReferenceRHI);

		}, TStatId(), nullptr, ENamedThreads::ActualRenderingThread
	);
	UpdateResourceTask->Wait();
}

void JJTextureImporter::CopyTexture(const FString& SrcFileName, const FString& DestFileName)
{
	if(!FPlatformFileManager::Get().GetPlatformFile().DirectoryExists(*FPaths::GetPath(DestFileName)))
	{
		FPlatformFileManager::Get().GetPlatformFile().CreateDirectory(*FPaths::GetPath(DestFileName));
	}
	if(FPlatformFileManager::Get().GetPlatformFile().FileExists(*SrcFileName))
		FPlatformFileManager::Get().GetPlatformFile().CopyFile(*DestFileName, *SrcFileName);
}
