#pragma once
#include "RenderData.h"


class TextureResource : public std::enable_shared_from_this<TextureResource>
{
public:
	TextureResource();
	~TextureResource();
	void initialize(
		size_t id,
		uint32_t width,
		uint32_t height,
		void* pixels,
		PIXEL_FORMAT pixelFormat,
		uint32_t miplevels);


public:
	TextureBufferResource mTextureBufferResource;
	
private:
	vk::Sampler createTextureSampler(uint32_t mipLevels);
	void createDescriptorSet();

private:
	size_t mId;
	bool mInit;
};


