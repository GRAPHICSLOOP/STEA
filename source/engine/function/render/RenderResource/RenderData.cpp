#include "RenderData.h"
#include "../VulkanUtil.h"
#include "function/global/RuntimeGlobalContext.h"

ObjectBufferResource::ObjectBufferResource()
{

}

ObjectBufferResource::ObjectBufferResource(vk::DeviceSize bufferDataSize)
{
    mBufferDataSize = bufferDataSize;
    spawnBuffer(mBufferDataSize);
    createDescriptorSet(mBufferDataSize);
}

ObjectBufferResource::~ObjectBufferResource()
{
    vkUnmapMemory(gRuntimeGlobalContext.getRHI()->mDevice, mMemory);
    gRuntimeGlobalContext.getRHI()->mDevice.destroyBuffer(mBuffer);
    gRuntimeGlobalContext.getRHI()->mDevice.freeMemory(mMemory);
}

void ObjectBufferResource::spawnBuffer(vk::DeviceSize bufferDataSize)
{
    VulkanUtil::createBuffer(
        bufferDataSize,
        vk::BufferUsageFlagBits::eUniformBuffer,
        vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostCoherent,
        mBuffer,
        mMemory
    );

    // 不立马解除映射 销毁时才解除
    vkMapMemory(gRuntimeGlobalContext.getRHI()->mDevice, mMemory, 0, sizeof(ObjectBufferData), 0, &mData);
}

void ObjectBufferResource::updateData(void* data)
{
    memcpy(mData, data, mBufferDataSize);
}

void ObjectBufferResource::createDescriptorSet(vk::DeviceSize bufferDataSize)
{
    vk::DescriptorSetLayout layout = gRuntimeGlobalContext.getRenderResource()->getDescriptorSetLayout(DESCRIPTOR_TYPE::DESCRIPTOR_TYPE_OBJECTUNIFORM);

    vk::DescriptorSetAllocateInfo info;
    info.descriptorPool = gRuntimeGlobalContext.getRHI()->mDescriptorPool;
    info.descriptorSetCount = 1;
    info.pSetLayouts = &layout;
    mDescriptorSet = gRuntimeGlobalContext.getRHI()->mDevice.allocateDescriptorSets(info)[0];

    vk::DescriptorBufferInfo objectBufferInfo;
    objectBufferInfo.buffer = mBuffer;
    objectBufferInfo.offset = 0;
    objectBufferInfo.range = bufferDataSize;


    // 更新描述符
    std::array<vk::WriteDescriptorSet, 1> writeSet;
    writeSet[0].dstArrayElement = 0;
    writeSet[0].dstBinding = 0;
    writeSet[0].dstSet = mDescriptorSet;
    writeSet[0].descriptorType = vk::DescriptorType::eUniformBuffer;
    writeSet[0].descriptorCount = 1;
    writeSet[0].pBufferInfo = &objectBufferInfo;

    gRuntimeGlobalContext.getRHI()->mDevice.updateDescriptorSets(writeSet, nullptr);

}

CameraBufferResource::CameraBufferResource()
{
    mData = &privateData;
}
