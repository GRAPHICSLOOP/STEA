﻿#include "IndexResource.h"
#include "../VulkanUtil.h"
#include "../../global/RuntimeGlobalContext.h"

IndexResource::IndexResource()
{
    mIndexCount = 0;
}

IndexResource::~IndexResource()
{
    gRuntimeGlobalContext.getRHI()->mDevice.destroyBuffer(mBuffer);
    gRuntimeGlobalContext.getRHI()->mDevice.freeMemory(mBufferMemory);
}

std::shared_ptr<IndexResource> IndexResource::create(const void* indicesData, uint32_t count)
{
    std::shared_ptr<IndexResource> indexResource = std::make_shared<IndexResource>();

    indexResource->mIndexCount = count;
    size_t size = count * sizeof(uint32_t);

    vk::Buffer stagingBuffer;
    vk::DeviceMemory stagingBufferMemory;
    VulkanUtil::createBuffer(
        size,
        vk::BufferUsageFlagBits::eTransferSrc,
        vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostCoherent,
        stagingBuffer,
        stagingBufferMemory);


    void* data;
    vkMapMemory(gRuntimeGlobalContext.getRHI()->mDevice, stagingBufferMemory, 0, size, 0, &data);
    memcpy(data, indicesData, size);
    vkUnmapMemory(gRuntimeGlobalContext.getRHI()->mDevice, stagingBufferMemory);

    VulkanUtil::createBuffer(
        size,
        vk::BufferUsageFlagBits::eTransferDst | vk::BufferUsageFlagBits::eIndexBuffer,
        vk::MemoryPropertyFlagBits::eDeviceLocal,
        indexResource->mBuffer,
        indexResource->mBufferMemory);

    VulkanUtil::copyBuffer(stagingBuffer, indexResource->mBuffer, size);

    gRuntimeGlobalContext.getRHI()->mDevice.destroyBuffer(stagingBuffer);
    gRuntimeGlobalContext.getRHI()->mDevice.freeMemory(stagingBufferMemory);

    return indexResource;
}
