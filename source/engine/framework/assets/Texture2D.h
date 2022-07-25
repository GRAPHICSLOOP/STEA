#pragma once
#include "function/render/renderResource/ImageResource.h"

class Texture2D
{
public:
	~Texture2D();
	void initialize(std::string path);
	void loadTexture();
	ImageResource* getTextureResource();
	size_t getId() { return mId; }
public:
	uint32_t mWidth;
	uint32_t mHeight;

private:
	std::shared_ptr<ImageResource> mTextureResource;
	std::string mPath;
	uint32_t mMiplevels;
	vk::Format mFormat;
	size_t mId;
};


