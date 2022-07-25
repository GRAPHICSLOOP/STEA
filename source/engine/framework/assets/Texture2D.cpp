#include "Texture2D.h"
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#include "core/base/macro.h"
#include "function/global/RuntimeGlobalContext.h"
#include "function/render/VulkanUtil.h"
#include "function/render/RenderResource.h"
#include <stdlib.h>

Texture2D::~Texture2D()
{
    auto iter = gRuntimeGlobalContext.getRenderResource()->mGlobalTextureResources.find(mId);
    if(iter != gRuntimeGlobalContext.getRenderResource()->mGlobalTextureResources.end())
        gRuntimeGlobalContext.getRenderResource()->mGlobalTextureResources.erase(iter);
}

void Texture2D::initialize(std::string path)
{
	std::hash<std::string> hash_fn;
	mId = hash_fn(path);
    mPath = path;
	
    loadTexture();
}

void Texture2D::loadTexture()
{
    int texWidth, texHeight, texChannels;
    stbi_uc* pixels = stbi_load(mPath.c_str(), &texWidth, &texHeight, &texChannels, STBI_rgb_alpha);
    CHECK_NULL(pixels);
    vk::DeviceSize DeviceSize = (uint64_t)texWidth * texHeight * 4;
    uint32_t miplevels = (uint32_t)std::floor(std::log2(std::max(texWidth, texHeight))) + 1;
    
    mFormat = vk::Format::eR8G8B8A8Unorm;
    mWidth = texWidth;
    mHeight = texHeight;
    mMiplevels = miplevels;

    // 检查是否有重复资源
    auto iter = gRuntimeGlobalContext.getRenderResource()->mGlobalTextureResources.find(mId);
    if (iter != gRuntimeGlobalContext.getRenderResource()->mGlobalTextureResources.end())
    {
        STEALOG_INFO("重复贴图不加载：{}", mId);
    }
    else
    {
        mTextureResource = ImageResource::createTextureResource
        (
            mWidth,
            mHeight,
            vk::ImageUsageFlagBits::eSampled,
            pixels,
            mFormat,
            true
        );

        gRuntimeGlobalContext.getRenderResource()->mGlobalTextureResources[mId] = mTextureResource.get();
    }

    stbi_image_free(pixels);
    return;
}

ImageResource* Texture2D::getTextureResource()
{
    return gRuntimeGlobalContext.getRenderResource()->mGlobalTextureResources[mId];
}
