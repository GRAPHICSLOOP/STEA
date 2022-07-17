#include "ImageResource.h"
#include "function/global/RuntimeGlobalContext.h"
#include "function/render/VulkanRHI.h"
#include "function/render/VulkanUtil.h"
#include "core/base/macro.h"

ImageResource::ImageResource()
{
}

ImageResource::~ImageResource()
{
    gRuntimeGlobalContext.getRHI()->mDevice.destroyImage(mImageBufferResource.mImage);
    gRuntimeGlobalContext.getRHI()->mDevice.destroyImageView(mImageBufferResource.mImageInfo.imageView);
    gRuntimeGlobalContext.getRHI()->mDevice.freeMemory(mImageBufferResource.mMemory);
    gRuntimeGlobalContext.getRHI()->mDevice.destroySampler(mImageBufferResource.mImageInfo.sampler);
}

void ImageResource::getPixelInfo(const vk::Format pixelFormat, const uint32_t width, const uint32_t height, size_t& pixelSize)
{
    switch (pixelFormat)
    {
    case vk::Format::eR8G8B8Unorm:
        pixelSize = (size_t)width * height * 3;
        break;
    case vk::Format::eB8G8R8A8Unorm:
        pixelSize = (size_t)width * height * 4;
        break;
    case vk::Format::eR8G8B8Srgb:
        pixelSize = (size_t)width * height * 3;
        break;
    case vk::Format::eR8G8B8A8Unorm:
        pixelSize = (size_t)width * height * 4;
        break;
    case vk::Format::eR8G8B8A8Srgb:
        pixelSize = (size_t)width * height * 4;
        break;
    case vk::Format::eR32G32Sfloat:
        pixelSize = (size_t)width * height * 4 * 2;
        break;
    case vk::Format::eR32G32B32Sfloat:
        pixelSize = (size_t)width * height * 4 * 3;
        break;
    case vk::Format::eR32G32B32A32Sfloat:
        pixelSize = (size_t)width * height * 4 * 4;
        break;
    case vk::Format::eD24UnormS8Uint:
        pixelSize = (size_t)width * height * 4;
        break;
    default:
        throw std::runtime_error("invalid pixelSize");
        break;
    }
}

void ImageResource::setImageLayout(vk::CommandBuffer cmdBuffer, ImageBufferResource* image, vk::ImageLayout dstLayout, const vk::ImageSubresourceRange& subresourceRange)
{
    vk::ImageMemoryBarrier imageBarrier;
    imageBarrier.image = image->mImage;
    imageBarrier.subresourceRange = subresourceRange;
    imageBarrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
    imageBarrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
    imageBarrier.oldLayout = image->mImageInfo.imageLayout;
    imageBarrier.newLayout = dstLayout;
    vk::PipelineStageFlags srcStage;
    vk::PipelineStageFlags dstStage;
    setImageBarrierInfo(imageBarrier.srcAccessMask, imageBarrier.oldLayout, srcStage);
    setImageBarrierInfo(imageBarrier.dstAccessMask, imageBarrier.newLayout, dstStage);

    vk::DependencyFlags dependencyFlags;
    cmdBuffer.pipelineBarrier(
        srcStage, dstStage,
        dependencyFlags,
        0, nullptr,
        0, nullptr,
        1, &imageBarrier);

    image->mImageInfo.imageLayout = dstLayout;
}

void ImageResource::setImageLayout(vk::CommandBuffer cmdBuffer, vk::Image image, vk::ImageLayout srcLayout, vk::ImageLayout dstLayout, const vk::ImageSubresourceRange& subresourceRange)
{
    vk::ImageMemoryBarrier imageBarrier;
    imageBarrier.image = image;
    imageBarrier.subresourceRange = subresourceRange;
    imageBarrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
    imageBarrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
    imageBarrier.oldLayout = srcLayout;
    imageBarrier.newLayout = dstLayout;
    vk::PipelineStageFlags srcStage;
    vk::PipelineStageFlags dstStage;
    setImageBarrierInfo(imageBarrier.srcAccessMask, imageBarrier.oldLayout, srcStage);
    setImageBarrierInfo(imageBarrier.dstAccessMask, imageBarrier.newLayout, dstStage);

    vk::DependencyFlags dependencyFlags;
    cmdBuffer.pipelineBarrier(
        srcStage, dstStage,
        dependencyFlags,
        0, nullptr,
        0, nullptr,
        1, &imageBarrier);
}

void ImageResource::setImageBarrierInfo(vk::AccessFlags& accessFlags, vk::ImageLayout targetLayout, vk::PipelineStageFlags& stages)
{
    switch (targetLayout)
    {
    case vk::ImageLayout::eUndefined:
        accessFlags = vk::AccessFlagBits::eNone;
        stages = vk::PipelineStageFlagBits::eTopOfPipe;
        break;
    case vk::ImageLayout::eTransferDstOptimal:
        accessFlags = vk::AccessFlagBits::eTransferWrite;
        stages = vk::PipelineStageFlagBits::eTransfer;
        break;
    case vk::ImageLayout::eColorAttachmentOptimal:
        accessFlags = vk::AccessFlagBits::eColorAttachmentWrite;
        stages = vk::PipelineStageFlagBits::eColorAttachmentOutput;
        break;
    case vk::ImageLayout::eDepthStencilAttachmentOptimal:
        break;
    case vk::ImageLayout::eTransferSrcOptimal:
        accessFlags = vk::AccessFlagBits::eTransferRead;
        stages = vk::PipelineStageFlagBits::eTransfer;
        break;
    case vk::ImageLayout::ePresentSrcKHR:
        STEALOG_WARN("ePresentSrcKHR 未定义！");
        break;
    case vk::ImageLayout::eShaderReadOnlyOptimal:
        accessFlags = vk::AccessFlagBits::eShaderRead;
        stages = vk::PipelineStageFlagBits::eFragmentShader;
        break;
    default:
        STEALOG_WARN("未知未定义！");
        break;
    }
}

std::shared_ptr<ImageResource> ImageResource::createTextureResource(
    uint32_t width, 
    uint32_t height, 
    vk::ImageUsageFlags usage, 
    void* pixels, 
    vk::Format pixelFormat, 
    bool miplevel,
    vk::DescriptorSetLayout setLayout)
{
    // 确定是否需要mipmap
    uint32_t miplevels = 1;
    if(miplevel)
        uint32_t miplevels = (uint32_t)std::floor(std::log2(std::max(width, height))) + 1;

    std::shared_ptr<ImageResource> imageResource = std::make_shared<ImageResource>();
    ImageBufferResource& imageBufferResource = imageResource->mImageBufferResource;
    size_t pixelSize = 0;
    getPixelInfo(pixelFormat, width, height, pixelSize);

    vk::Buffer stagingBuffer;
    vk::DeviceMemory stagingBufferMemory;

    // 首先将数据上传到GPU
    {
        VulkanUtil::createBuffer(
            pixelSize,
            vk::BufferUsageFlagBits::eTransferSrc,
            vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostCoherent,
            stagingBuffer,
            stagingBufferMemory);

        void* data;
        vkMapMemory(gRuntimeGlobalContext.getRHI()->mDevice, stagingBufferMemory, 0, pixelSize, 0, &data);
        memcpy(data, pixels, pixelSize);
        vkUnmapMemory(gRuntimeGlobalContext.getRHI()->mDevice, stagingBufferMemory);
    }

    // 创建Image
    {
        // 加上传输目标，因为需要将数据传送进来
        if (!(usage & vk::ImageUsageFlagBits::eTransferDst))
            usage |= vk::ImageUsageFlagBits::eTransferDst;

        // 加上传输源，因为需要读取然后创建minmap
        if (miplevel && !(usage & vk::ImageUsageFlagBits::eTransferSrc))
            usage |= vk::ImageUsageFlagBits::eTransferSrc;

        // 创建image
        vk::ImageCreateInfo imageCreateInfo;
        imageCreateInfo.imageType = vk::ImageType::e2D;
        imageCreateInfo.format = pixelFormat;
        imageCreateInfo.mipLevels = miplevels;
        imageCreateInfo.arrayLayers = 1;
        imageCreateInfo.samples = vk::SampleCountFlagBits::e1;
        imageCreateInfo.tiling = vk::ImageTiling::eOptimal;
        imageCreateInfo.sharingMode = vk::SharingMode::eExclusive;
        imageCreateInfo.initialLayout = vk::ImageLayout::eUndefined;
        imageCreateInfo.extent = vk::Extent3D(width, height, 1);
        imageCreateInfo.usage = usage;
        imageBufferResource.mImage = gRuntimeGlobalContext.getRHI()->mDevice.createImage(imageCreateInfo);

        // bind image buffer
        vk::MemoryRequirements memRequirements;
        gRuntimeGlobalContext.getRHI()->mDevice.getImageMemoryRequirements(imageBufferResource.mImage, &memRequirements);
        vk::MemoryAllocateInfo allocInfo;
        allocInfo.allocationSize = memRequirements.size;
        allocInfo.memoryTypeIndex = VulkanUtil::findMemoryType(memRequirements.memoryTypeBits, vk::MemoryPropertyFlagBits::eDeviceLocal);
        vk::Result result = gRuntimeGlobalContext.getRHI()->mDevice.allocateMemory(&allocInfo, nullptr, &imageBufferResource.mMemory);
        CHECK_NULL(imageBufferResource.mMemory);
        gRuntimeGlobalContext.getRHI()->mDevice.bindImageMemory(imageBufferResource.mImage, imageBufferResource.mMemory, 0);
    }

    // record
    {
        // start record
        vk::CommandBuffer cmdBuffer = gRuntimeGlobalContext.getRHI()->beginSingleTimeBuffer();

        vk::ImageSubresourceRange subresourceRange = {};
        subresourceRange.aspectMask = vk::ImageAspectFlagBits::eColor;
        subresourceRange.levelCount = 1;
        subresourceRange.layerCount = 1;
        subresourceRange.baseArrayLayer = 0;
        subresourceRange.baseMipLevel = 0;

        // undefined to TransferDest
        setImageLayout(cmdBuffer, &imageBufferResource, vk::ImageLayout::eTransferDstOptimal, subresourceRange);

        // copy buffer to image
        vk::BufferImageCopy bufferCopyRegion;
        bufferCopyRegion.imageSubresource.aspectMask = vk::ImageAspectFlagBits::eColor;
        bufferCopyRegion.imageSubresource.mipLevel = 0;
        bufferCopyRegion.imageSubresource.baseArrayLayer = 0;
        bufferCopyRegion.imageSubresource.layerCount = 1;
        bufferCopyRegion.imageExtent.width = width;
        bufferCopyRegion.imageExtent.height = height;
        bufferCopyRegion.imageExtent.depth = 1;
        cmdBuffer.copyBufferToImage(stagingBuffer, imageBufferResource.mImage, vk::ImageLayout::eTransferDstOptimal, 1, &bufferCopyRegion);

        // TransferDest to TransferSrc
        setImageLayout(cmdBuffer, &imageBufferResource, vk::ImageLayout::eTransferSrcOptimal, subresourceRange);

        // Generate the mip chain
        for (uint32_t i = 1; i < (uint32_t)miplevels; i++)
        {
            vk::ImageBlit imageBlit;

            uint32_t mip0Width = std::max(width >> (i - 1), (uint32_t)1);
            uint32_t mip0Height = std::max(height >> (i - 1), (uint32_t)1);
            uint32_t mip1Width = std::max(width >> (i - 0), (uint32_t)1);
            uint32_t mip1Height = std::max(height >> (i - 0), (uint32_t)1);

            imageBlit.srcSubresource.aspectMask = vk::ImageAspectFlagBits::eColor;
            imageBlit.srcSubresource.layerCount = 1;
            imageBlit.srcSubresource.mipLevel = i - 1;
            imageBlit.srcOffsets[1].x = int32_t(mip0Width);
            imageBlit.srcOffsets[1].y = int32_t(mip0Height);
            imageBlit.srcOffsets[1].z = 1;

            imageBlit.dstSubresource.aspectMask = vk::ImageAspectFlagBits::eColor;
            imageBlit.dstSubresource.layerCount = 1;
            imageBlit.dstSubresource.mipLevel = i;
            imageBlit.dstOffsets[1].x = int32_t(mip1Width);
            imageBlit.dstOffsets[1].y = int32_t(mip1Height);
            imageBlit.dstOffsets[1].z = 1;

            vk::ImageSubresourceRange mipSubRange = {};
            mipSubRange.aspectMask = vk::ImageAspectFlagBits::eColor;
            mipSubRange.baseMipLevel = i;
            mipSubRange.levelCount = 1;
            mipSubRange.layerCount = 1;
            mipSubRange.baseArrayLayer = 0;

            // undefined to dst
            setImageLayout(cmdBuffer, imageBufferResource.mImage, vk::ImageLayout::eUndefined, vk::ImageLayout::eTransferDstOptimal, mipSubRange);

            // blit image
            cmdBuffer.blitImage(
                imageBufferResource.mImage,
                vk::ImageLayout::eTransferSrcOptimal,
                imageBufferResource.mImage, vk::ImageLayout::eTransferDstOptimal,
                imageBlit,
                vk::Filter::eLinear);

            // dst to src
            setImageLayout(cmdBuffer, imageBufferResource.mImage, vk::ImageLayout::eTransferDstOptimal, vk::ImageLayout::eTransferSrcOptimal, mipSubRange);
        }
        subresourceRange.levelCount = miplevels;

        // dst to layout
        setImageLayout(cmdBuffer, &imageBufferResource, vk::ImageLayout::eShaderReadOnlyOptimal, subresourceRange);

        // end record
        gRuntimeGlobalContext.getRHI()->endSingleTimeBuffer(cmdBuffer);
    }

    // sample
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
        info.maxLod = (float)miplevels;
        imageBufferResource.mImageInfo.sampler = gRuntimeGlobalContext.getRHI()->mDevice.createSampler(info);
        CHECK_NULL(imageBufferResource.mImageInfo.sampler);
    }

    // imageview
    {
        vk::ImageViewCreateInfo viewInfo;
        viewInfo.image = imageBufferResource.mImage;
        viewInfo.viewType = vk::ImageViewType::e2D;
        viewInfo.format = pixelFormat;
        viewInfo.components = { VK_COMPONENT_SWIZZLE_R, VK_COMPONENT_SWIZZLE_G, VK_COMPONENT_SWIZZLE_B, VK_COMPONENT_SWIZZLE_A };
        viewInfo.subresourceRange.aspectMask = vk::ImageAspectFlagBits::eColor;
        viewInfo.subresourceRange.layerCount = 1;
        viewInfo.subresourceRange.levelCount = miplevels;
        imageBufferResource.mImageInfo.imageView = gRuntimeGlobalContext.getRHI()->mDevice.createImageView(viewInfo);
        CHECK_NULL(imageBufferResource.mImageInfo.imageView);
    }

    // create set
    imageResource->createDescriptorSet(setLayout);

    // clear
    gRuntimeGlobalContext.getRHI()->mDevice.destroyBuffer(stagingBuffer);
    gRuntimeGlobalContext.getRHI()->mDevice.freeMemory(stagingBufferMemory);

    imageBufferResource.mFormat = pixelFormat;
    return imageResource;
}

std::shared_ptr<ImageResource> ImageResource::createAttachment(uint32_t width, uint32_t height, vk::ImageUsageFlags usage, vk::ImageAspectFlags aspectFlags, vk::Format pixelFormat,vk::DescriptorSetLayout setLayout)
{
    uint32_t miplevels = 1;
    std::shared_ptr<ImageResource> imageResource = std::make_shared<ImageResource>();
    ImageBufferResource& imageBufferResource = imageResource->mImageBufferResource;
    size_t pixelSize = 0;
    getPixelInfo(pixelFormat, width, height, pixelSize);

    // 创建Image
    {
        vk::ImageCreateInfo imageCreateInfo;
        imageCreateInfo.imageType = vk::ImageType::e2D;
        imageCreateInfo.format = pixelFormat;
        imageCreateInfo.mipLevels = miplevels;
        imageCreateInfo.arrayLayers = 1;
        imageCreateInfo.samples = vk::SampleCountFlagBits::e1;
        imageCreateInfo.tiling = vk::ImageTiling::eOptimal;
        imageCreateInfo.sharingMode = vk::SharingMode::eExclusive;
        imageCreateInfo.initialLayout = vk::ImageLayout::eUndefined;
        imageCreateInfo.extent = vk::Extent3D(width, height, 1);
        imageCreateInfo.usage = usage;
        imageBufferResource.mImage = gRuntimeGlobalContext.getRHI()->mDevice.createImage(imageCreateInfo);

        // bind image buffer
        vk::MemoryRequirements memRequirements;
        gRuntimeGlobalContext.getRHI()->mDevice.getImageMemoryRequirements(imageBufferResource.mImage, &memRequirements);
        vk::MemoryAllocateInfo allocInfo;
        allocInfo.allocationSize = memRequirements.size;
        allocInfo.memoryTypeIndex = VulkanUtil::findMemoryType(memRequirements.memoryTypeBits, vk::MemoryPropertyFlagBits::eDeviceLocal);
        vk::Result result = gRuntimeGlobalContext.getRHI()->mDevice.allocateMemory(&allocInfo, nullptr, &imageBufferResource.mMemory);
        CHECK_NULL(imageBufferResource.mMemory);
        gRuntimeGlobalContext.getRHI()->mDevice.bindImageMemory(imageBufferResource.mImage, imageBufferResource.mMemory, 0);
    }

    // sample
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
        info.maxLod = (float)miplevels;
        imageBufferResource.mImageInfo.sampler = gRuntimeGlobalContext.getRHI()->mDevice.createSampler(info);
        CHECK_NULL(imageBufferResource.mImageInfo.sampler);
    }

    // imageview
    {
        vk::ImageViewCreateInfo viewInfo;
        viewInfo.image = imageBufferResource.mImage;
        viewInfo.viewType = vk::ImageViewType::e2D;
        viewInfo.format = pixelFormat;
        viewInfo.components = { VK_COMPONENT_SWIZZLE_R, VK_COMPONENT_SWIZZLE_G, VK_COMPONENT_SWIZZLE_B, VK_COMPONENT_SWIZZLE_A };
        viewInfo.subresourceRange.aspectMask = aspectFlags;
        viewInfo.subresourceRange.layerCount = 1;
        viewInfo.subresourceRange.levelCount = miplevels;
        imageBufferResource.mImageInfo.imageView = gRuntimeGlobalContext.getRHI()->mDevice.createImageView(viewInfo);
        CHECK_NULL(imageBufferResource.mImageInfo.imageView);
    }

    imageBufferResource.mFormat = pixelFormat;
    return imageResource;
}

void ImageResource::createDescriptorSet(vk::DescriptorSetLayout setLayout)
{
    vk::DescriptorSetAllocateInfo info;
    info.descriptorPool = gRuntimeGlobalContext.getRHI()->mDescriptorPool;
    info.descriptorSetCount = 1;
    info.pSetLayouts = &setLayout;
    mImageBufferResource.mDescriptorSet = gRuntimeGlobalContext.getRHI()->mDevice.allocateDescriptorSets(info)[0];

    // 更新描述符
    std::array<vk::WriteDescriptorSet, 1> writeSet;
    writeSet[0].dstArrayElement = 0;
    writeSet[0].dstBinding = 0;
    writeSet[0].dstSet = mImageBufferResource.mDescriptorSet;
    writeSet[0].descriptorType = vk::DescriptorType::eCombinedImageSampler;
    writeSet[0].descriptorCount = 1;
    writeSet[0].pImageInfo = &mImageBufferResource.mImageInfo;

    gRuntimeGlobalContext.getRHI()->mDevice.updateDescriptorSets(writeSet, nullptr);
}

