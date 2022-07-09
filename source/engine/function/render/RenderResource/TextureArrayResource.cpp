#include "TextureArrayResource.h"
#include "../../global/RuntimeGlobalContext.h"
#include "../VulkanUtil.h"
#include "core/base/macro.h"

TextureArrayResource::TextureArrayResource()
{
    mMemoryOffset = 0;
}

TextureArrayResource::~TextureArrayResource()
{
}

void TextureArrayResource::initialize(uint32_t width, uint32_t height, PIXEL_FORMAT pixelFormat, bool mipLevels, uint32_t maxLayerLevels)
{
    mWidth = width;
    mHeight = height;
    mPixelFormat = pixelFormat;
    mMaxLayerLevels = maxLayerLevels;

    if (mipLevels)
        mMipLevels = (uint32_t)std::floor(std::log2(std::max(width, height))) + 1;
    else
        mMipLevels = 1;

    mTextureBufferResource = createTextureBufferResource();
    createDescriptorSet();

}

void TextureArrayResource::uploadData(uint8* data, uint32_t dataCount)
{
    // 确定总体要分配大多的texturearray
    VkDeviceSize pixelSize = 0;
    vk::Format vulkanImageFormat;
    switch (mPixelFormat)
    {
    case PIXEL_FORMAT::PIXEL_FORMAT_R8G8B8_UNORM:
        pixelSize = (size_t)mWidth * mHeight * 3;
        vulkanImageFormat = vk::Format::eR8G8B8Unorm;
        break;
    case PIXEL_FORMAT::PIXEL_FORMAT_R8G8B8_SRGB:
        pixelSize = (size_t)mWidth * mHeight * 3;
        vulkanImageFormat = vk::Format::eR8G8B8Srgb;
        break;
    case PIXEL_FORMAT::PIXEL_FORMAT_R8G8B8A8_UNORM:
        pixelSize = (size_t)mWidth * mHeight * 4;
        vulkanImageFormat = vk::Format::eR8G8B8A8Unorm;
        break;
    case PIXEL_FORMAT::PIXEL_FORMAT_R8G8B8A8_SRGB:
        pixelSize = (size_t)mWidth * mHeight * 4;
        vulkanImageFormat = vk::Format::eR8G8B8A8Srgb;
        break;
    case PIXEL_FORMAT::PIXEL_FORMAT_R32G32_FLOAT:
        pixelSize = (size_t)mWidth * mHeight * 4 * 2;
        vulkanImageFormat = vk::Format::eR32G32Sfloat;
        break;
    case PIXEL_FORMAT::PIXEL_FORMAT_R32G32B32_FLOAT:
        pixelSize = (size_t)mWidth * mHeight * 4 * 3;
        vulkanImageFormat = vk::Format::eR32G32B32Sfloat;
        break;
    case PIXEL_FORMAT::PIXEL_FORMAT_R32G32B32A32_FLOAT:
        pixelSize = (size_t)mWidth * mHeight * 4 * 4;
        vulkanImageFormat = vk::Format::eR32G32B32A32Sfloat;
        break;
    default:
        throw std::runtime_error("invalid pixelSize");
        break;
    }

    // 准备stagingBuffer
    vk::Buffer stagingBuffer;
    vk::DeviceMemory stagingBufferMemory;
    VulkanUtil::createBuffer(
        pixelSize * dataCount,
        vk::BufferUsageFlagBits::eTransferSrc,
        vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostCoherent,
        stagingBuffer,
        stagingBufferMemory
    );

    void* bufferData;
    vkMapMemory(gRuntimeGlobalContext.getRHI()->mDevice, stagingBufferMemory, 0, pixelSize, 0, &bufferData);
    for (uint32_t i = 0; i < dataCount; ++i)
    {
        VkDeviceSize Offset = pixelSize * i;
        memcpy(bufferData, data + Offset, pixelSize);
    }

    vkUnmapMemory(gRuntimeGlobalContext.getRHI()->mDevice, stagingBufferMemory);

    // start record
    vk::CommandBuffer commandBuffer = gRuntimeGlobalContext.getRHI()->mCommandBuffer;
    gRuntimeGlobalContext.getRHI()->beginSingleTimeBuffer();

    vk::ImageSubresourceRange subresourceRange = {};
    subresourceRange.aspectMask = vk::ImageAspectFlagBits::eColor;
    subresourceRange.levelCount = 1;
    subresourceRange.layerCount = dataCount;
    subresourceRange.baseMipLevel = 0;
    subresourceRange.baseArrayLayer = 0;

    // 转换为TransferDest布局，因为复制需要这个布局
    VulkanUtil::imagePipelineBarrier(commandBuffer, mTextureBufferResource.mImage, IMAGE_LAYOUT_BARRIER::Undefined, IMAGE_LAYOUT_BARRIER::TransferDest, subresourceRange);
    std::vector<vk::BufferImageCopy> bufferCopyRegions;
    for (uint32_t i = 0; i < dataCount; ++i)
    {
        vk::BufferImageCopy bufferCopyRegion = {};
        bufferCopyRegion.imageSubresource.mipLevel = 0;
        bufferCopyRegion.imageSubresource.baseArrayLayer = i; // 第几层
        bufferCopyRegion.imageSubresource.layerCount = 1; // 表示一次只复制一层
        bufferCopyRegion.imageExtent.width = mWidth;
        bufferCopyRegion.imageExtent.height = mHeight;
        bufferCopyRegion.imageExtent.depth = 1;
        bufferCopyRegion.bufferOffset = pixelSize * i;
        bufferCopyRegions.push_back(bufferCopyRegion);
    }

    commandBuffer.copyBufferToImage(stagingBuffer, mTextureBufferResource.mImage, vk::ImageLayout::eTransferDstOptimal, bufferCopyRegions);
    VulkanUtil::imagePipelineBarrier(commandBuffer, mTextureBufferResource.mImage, IMAGE_LAYOUT_BARRIER::TransferDest, IMAGE_LAYOUT_BARRIER::TransferSource, subresourceRange);

    // Generate the mip chain
    for (uint32_t i = 1; i < mMipLevels; i++)
    {
        vk::ImageBlit imageBlit = {};

        imageBlit.srcSubresource.layerCount = dataCount;
        imageBlit.srcSubresource.mipLevel = i - 1;
        imageBlit.srcOffsets[1].x = int32_t(mWidth >> (i - 1));
        imageBlit.srcOffsets[1].y = int32_t(mHeight >> (i - 1));
        imageBlit.srcOffsets[1].z = 1;

        imageBlit.dstSubresource.layerCount = dataCount;
        imageBlit.dstSubresource.mipLevel = i;
        imageBlit.dstOffsets[1].x = int32_t(mWidth >> i);
        imageBlit.dstOffsets[1].y = int32_t(mHeight >> i);
        imageBlit.dstOffsets[1].z = 1;

        vk::ImageSubresourceRange mipSubRange = {};
        mipSubRange.aspectMask = vk::ImageAspectFlagBits::eColor;
        mipSubRange.baseMipLevel = i;
        mipSubRange.levelCount = 1;
        mipSubRange.layerCount = dataCount;
        mipSubRange.baseArrayLayer = 0;

        VulkanUtil::imagePipelineBarrier(commandBuffer, mTextureBufferResource.mImage, IMAGE_LAYOUT_BARRIER::Undefined, IMAGE_LAYOUT_BARRIER::TransferDest, mipSubRange);
        commandBuffer.blitImage(mTextureBufferResource.mImage, vk::ImageLayout::eTransferSrcOptimal, mTextureBufferResource.mImage, vk::ImageLayout::eTransferDstOptimal, imageBlit, vk::Filter::eLinear);
        VulkanUtil::imagePipelineBarrier(commandBuffer, mTextureBufferResource.mImage, IMAGE_LAYOUT_BARRIER::TransferDest, IMAGE_LAYOUT_BARRIER::TransferSource, mipSubRange);
    }

    subresourceRange.aspectMask = vk::ImageAspectFlagBits::eColor;
    subresourceRange.levelCount = mMipLevels;
    subresourceRange.layerCount = dataCount;
    subresourceRange.baseMipLevel = 0;
    VulkanUtil::imagePipelineBarrier(commandBuffer, mTextureBufferResource.mImage, IMAGE_LAYOUT_BARRIER::TransferDest, IMAGE_LAYOUT_BARRIER::PixelShaderRead, subresourceRange);

    gRuntimeGlobalContext.getRHI()->endSingleTimeBuffer(commandBuffer);
    gRuntimeGlobalContext.getRHI()->mDevice.destroyBuffer(stagingBuffer);
    gRuntimeGlobalContext.getRHI()->mDevice.freeMemory(stagingBufferMemory);
}

TextureBufferResource TextureArrayResource::createTextureBufferResource()
{
    TextureBufferResource textureBufferResource;

    // 确定总体要分配大多的texturearray
    size_t pixelSize = 0;
    vk::Format vulkanImageFormat;
    switch (mPixelFormat)
    {
    case PIXEL_FORMAT::PIXEL_FORMAT_R8G8B8_UNORM:
        pixelSize = (size_t)mWidth * mHeight * 3 * mMipLevels;
        vulkanImageFormat = vk::Format::eR8G8B8Unorm;
        break;
    case PIXEL_FORMAT::PIXEL_FORMAT_R8G8B8_SRGB:
        pixelSize = (size_t)mWidth * mHeight * 3 * mMipLevels;
        vulkanImageFormat = vk::Format::eR8G8B8Srgb;
        break;
    case PIXEL_FORMAT::PIXEL_FORMAT_R8G8B8A8_UNORM:
        pixelSize = (size_t)mWidth * mHeight * 4 * mMipLevels;
        vulkanImageFormat = vk::Format::eR8G8B8A8Unorm;
        break;
    case PIXEL_FORMAT::PIXEL_FORMAT_R8G8B8A8_SRGB:
        pixelSize = (size_t)mWidth * mHeight * 4 * mMipLevels;
        vulkanImageFormat = vk::Format::eR8G8B8A8Srgb;
        break;
    case PIXEL_FORMAT::PIXEL_FORMAT_R32G32_FLOAT:
        pixelSize = (size_t)mWidth * mHeight * 4 * 2 * mMipLevels;
        vulkanImageFormat = vk::Format::eR32G32Sfloat;
        break;
    case PIXEL_FORMAT::PIXEL_FORMAT_R32G32B32_FLOAT:
        pixelSize = (size_t)mWidth * mHeight * 4 * 3 * mMipLevels;
        vulkanImageFormat = vk::Format::eR32G32B32Sfloat;
        break;
    case PIXEL_FORMAT::PIXEL_FORMAT_R32G32B32A32_FLOAT:
        pixelSize = (size_t)mWidth * mHeight * 4 * 4 * mMipLevels;
        vulkanImageFormat = vk::Format::eR32G32B32A32Sfloat;
        break;
    default:
        throw std::runtime_error("invalid pixelSize");
        break;
    }

    // 创建image
    VulkanUtil::createImage(
        mWidth,
        mHeight,
        vulkanImageFormat,
        vk::ImageTiling::eOptimal,
        vk::ImageUsageFlagBits::eTransferDst | vk::ImageUsageFlagBits::eSampled | vk::ImageUsageFlagBits::eTransferSrc,
        vk::MemoryPropertyFlagBits::eDeviceLocal,
        textureBufferResource.mImage,
        textureBufferResource.mMemory,
        mMipLevels,
        vk::SampleCountFlagBits::e1,
        mMaxLayerLevels);

    // 创建imageSampler
    vk::SamplerCreateInfo samplerInfo;
    samplerInfo.minFilter = vk::Filter::eLinear;
    samplerInfo.magFilter = vk::Filter::eLinear;
    samplerInfo.addressModeU = vk::SamplerAddressMode::eRepeat;
    samplerInfo.addressModeV = vk::SamplerAddressMode::eRepeat;
    samplerInfo.addressModeW = vk::SamplerAddressMode::eRepeat;
    samplerInfo.anisotropyEnable = VK_FALSE;
    samplerInfo.maxAnisotropy = 1;
    samplerInfo.borderColor = vk::BorderColor::eIntOpaqueBlack;
    samplerInfo.unnormalizedCoordinates = VK_FALSE;
    samplerInfo.compareEnable = VK_FALSE;
    samplerInfo.compareOp = vk::CompareOp::eAlways;
    samplerInfo.mipmapMode = vk::SamplerMipmapMode::eLinear;
    samplerInfo.mipLodBias = 0;
    samplerInfo.minLod = 0;
    samplerInfo.maxLod = (float)mMipLevels;
    textureBufferResource.mTextureSampler = gRuntimeGlobalContext.getRHI()->mDevice.createSampler(samplerInfo);
    CHECK_NULL(textureBufferResource.mTextureSampler);

    vk::ImageSubresourceRange range;
    range.aspectMask = vk::ImageAspectFlagBits::eColor;
    range.baseArrayLayer = 0;
    range.baseMipLevel = 0;
    range.layerCount = mMaxLayerLevels;
    range.levelCount = mMipLevels;
    vk::ImageViewCreateInfo imageViewInfo;
    imageViewInfo.components = vk::ComponentMapping();
    imageViewInfo.format = vulkanImageFormat;
    imageViewInfo.image = textureBufferResource.mImage;
    imageViewInfo.viewType = vk::ImageViewType::e2DArray;
    imageViewInfo.subresourceRange = range;

    textureBufferResource.mImageView = gRuntimeGlobalContext.getRHI()->mDevice.createImageView(imageViewInfo);
    CHECK_NULL(textureBufferResource.mImageView);

    return textureBufferResource;
}

void TextureArrayResource::createDescriptorSet()
{
    vk::DescriptorSetLayout setLayout = gRuntimeGlobalContext.getRenderSystem()->mRenderResource->getDescriptorSetLayout(DT_Sample);

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