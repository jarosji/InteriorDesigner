#pragma once

#include "RHIResources.h"
#include "RHIDefinitions.h"

class UTexture2D;

class JJFILEIMPORTER_API JJTextureImporter
{
	
public:
	void ImportTexture(const FString& BaseFilename, UTexture2D*& NewTexture);
	void CopyTexture(const FString& SrcFileName, const FString& DestFileName);

private:
	FTexture2DRHIRef RHITexture2D;
	
	EPixelFormat RawImageFormatToPixelFormat(ERawImageFormat::Type RawImageFormat);

	class FTexture2DResource : public FTextureResource
	{
		public:
			FTexture2DResource(UTexture2D* InOwner, FTextureRHIRef InRHITextureRef);
			virtual ~FTexture2DResource() override;

			virtual uint32 GetSizeX() const override { return SizeX; }
			virtual uint32 GetSizeY() const override { return SizeY; }
		
			virtual void InitRHI() override;
			virtual void ReleaseRHI() override;

		protected:
			UTexture* Owner;
			uint32 SizeX;
			uint32 SizeY;
	};
	
	struct FTextureDataResource : FResourceBulkDataInterface
    {
		FTextureDataResource(void* InMipData, int32 InDataSize)
			: MipData(InMipData), DataSize(InDataSize)
		{
		}
	    
		virtual const void* GetResourceBulkData() const override { return MipData; }
		virtual uint32 GetResourceBulkDataSize() const override { return DataSize; }
		virtual void Discard() override {}
	    
	    private:
    		void* MipData;
    		int32 DataSize;
    };
};
