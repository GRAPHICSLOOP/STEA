﻿#include "TextureResource.h"
#include "function/global/RuntimeGlobalContext.h"
#include "function/render/VulkanRHI.h"
#include "function/render/VulkanUtil.h"
#include "core/base/macro.h"

TextureResource::TextureResource()
{
    mId = 0;
    mInit = false;
}

void TextureResource::initialize(
    size_t id,
    uint32_t width,
    uint32_t height,
    void* pixels,
    PIXEL_FORMAT pixelFormat,
    uint32_t miplevels)
{
    mId = id;

    if (gRuntimeGlobalContext.getRenderResource()->mGlobalTextureResources[mId].lock())
    {
        STEALOG_INFO("重复贴图不加载：{}", mId);
        return;
    }
    gRuntimeGlobalContext.getRenderResource()->mGlobalTextureResources[mId] = shared_from_this();
    mTextureBufferResource = VulkanUtil::createTextureBufferResource(width, height, pixels, pixelFormat, miplevels);
    mTextureBufferResource.mTextureSampler = createTextureSampler(miplevels);
    createDescriptorSet();
    mInit = true;
}

TextureResource::~TextureResource()
{
    if (!mInit)
        return;

    auto iter = gRuntimeGlobalContext.getRenderResource()->mGlobalTextureResources.find(mId);
    gRuntimeGlobalContext.getRenderResource()->mGlobalTextureResources.erase(iter);

    gRuntimeGlobalContext.getRHI()->mDevice.destroyImage(mTextureBufferResource.mImage);
    gRuntimeGlobalContext.getRHI()->mDevice.destroyImageView(mTextureBufferResource.mImageView);
    gRuntimeGlobalContext.getRHI()->mDevice.freeMemory(mTextureBufferResource.mMemory);
    gRuntimeGlobalContext.getRHI()->mDevice.destroySampler(mTextureBufferResource.mTextureSampler);
}

vk::Sampler TextureResource::createTextureSampler(uint32_t mipLevels)
{
    vk::SamplerCreateInfo info;
    info.minFilter = vk::Filter::eLinear;
    info.magFilter = vk::Filter::eLinear;
    info.addressModeU = vk::SamplerAddressMode::eRepeat;
    info.addressModeV = vk::SamplerAddressMode::eRepeat;
    info.addressModeW = vk::SamplerAddressMode::eRepeat;
    info.anisotropyEnable = VK_FALSE;
    info.maxAnisotropy = 1;
    info.borderColor = vk::BorderColor::eIntOpaqueBlack;
    info.unnormalizedCoordinates = VK_FALSE;
    info.compareEnable = VK_FALSE;
    info.compareOp = vk::CompareOp::eAlways;
    info.mipmapMode = vk::SamplerMipmapMode::eLinear;
    info.mipLodBias = 0;
    info.minLod = 0;
    info.maxLod = (float)mipLevels;

    vk::Sampler sample = gRuntimeGlobalContext.getRHI()->mDevice.createSampler(info);
    CHECK_NULL(sample);

    return sample;
}

void TextureResource::createDescriptorSet()
{
    vk::DescriptorSetLayout setLayout = gRuntimeGlobalContext.getRenderSystem()->mRenderResource->getDescriptorSetLayout(DESCRIPTOR_TYPE_SAMPLE);

    vk::DescriptorSetAllocateInfo info;
    info.descriptorPool = gRuntimeGlobalContext.getRHI()->mDescriptorPool;
    info.descriptorSetCount = 1;
    info.pSetLayouts = &setLayout;
    mTextureBufferResource.mDescriptorSet = gRuntimeGlobalContext.getRHI()->mDevice.allocateDescriptorSets(info)[0];

    vk::DescriptorImageInfo imageInfo;
    imageInfo.imageLayout = vk::ImageLayout::eShaderReadOnlyOptimal;
    imageInfo.imageView = mTextureBufferResource.mImageView;
    imageInfo.sampler = mTextureBufferResource.mTextureSampler;

    // 更新描述符
    std::array<vk::WriteDescriptorSet, 1> writeSet;
    writeSet[0].dstArrayElement = 0;
    writeSet[0].dstBinding = 0;
    writeSet[0].dstSet = mTextureBufferResource.mDescriptorSet;
    writeSet[0].descriptorType = vk::DescriptorType::eCombinedImageSampler;
    writeSet[0].descriptorCount = 1;
    writeSet[0].pImageInfo = &imageInfo;

    gRuntimeGlobalContext.getRHI()->mDevice.updateDescriptorSets(writeSet, nullptr);
}
