#pragma once
#include "RenderData.h"


class ImageResource
{
public:
	ImageResource();
	~ImageResource();
	void initialize(
		uint32_t width,
		uint32_t height,
		void* pixels,
		PIXEL_FORMAT pixelFormat,
		uint32_t miplevels);


public:
	ImageBufferResource mTextureBufferResource;
	
private:
	vk::Sampler createTextureSampler(uint32_t mipLevels);
	void createDescriptorSet();
};


