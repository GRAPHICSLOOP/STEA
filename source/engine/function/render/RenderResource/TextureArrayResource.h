#pragma once
#include "RenderData.h"
typedef unsigned char uint8;

class TextureArrayResource
{
public:
	TextureArrayResource();
	~TextureArrayResource();

	void initialize(
		uint32_t width,
		uint32_t height,
		PIXEL_FORMAT pixelFormat,
		bool mipLevels,
		uint32_t maxLayerLevels);

	void uploadData(uint8* data, uint32_t dataCount);

public:
	ImageBufferResource mTextureBufferResource;

private:
	ImageBufferResource createTextureBufferResource();
	void createDescriptorSet();

private:
	uint32_t mWidth;
	uint32_t mHeight;
	PIXEL_FORMAT mPixelFormat;
	uint32_t mMipLevels;
	uint32_t mMaxLayerLevels;
	VkDeviceSize mMemoryOffset;
};

