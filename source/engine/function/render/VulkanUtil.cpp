#include <fstream>
#include "VulkanUtil.h"
#include "function/global/RuntimeGlobalContext.h"
#include "core/base/macro.h"

uint32_t VulkanUtil::findMemoryType(uint32_t typeFilter, vk::MemoryPropertyFlags property)
{
    vk::PhysicalDevice phyDevice = gRuntimeGlobalContext.getRHI()->mPhyDevice;

    vk::PhysicalDeviceMemoryProperties memProperties;
    memProperties = phyDevice.getMemoryProperties();

    for (uint32_t i = 0; i < memProperties.memoryTypeCount; i++)
    {
        if (typeFilter & (1 << i) && (memProperties.memoryTypes[i].propertyFlags & property) == property)
        {
            return i;
        }
    }

    throw std::runtime_error("find memory faild");
}

void VulkanUtil::createBuffer(
    vk::DeviceSize size,
    vk::BufferUsageFlags usage,
    vk::MemoryPropertyFlags properties,
    vk::Buffer& buffer,
    vk::DeviceMemory& bufferMemory)
{
    vk::Device device = gRuntimeGlobalContext.getRHI()->mDevice;

    vk::BufferCreateInfo bufferInfo;
    bufferInfo.sharingMode = vk::SharingMode::eExclusive;
    bufferInfo.size = size;
    bufferInfo.usage = usage;
    buffer = device.createBuffer(bufferInfo);
    CHECK_NULL(buffer);

    vk::MemoryRequirements memRequirements;
    memRequirements = device.getBufferMemoryRequirements(buffer);

    vk::MemoryAllocateInfo allocInfo;
    allocInfo.setAllocationSize(memRequirements.size);
    allocInfo.setMemoryTypeIndex(findMemoryType(memRequirements.memoryTypeBits, properties));
    bufferMemory = device.allocateMemory(allocInfo);
    CHECK_NULL(bufferMemory);

    // 绑定buffer和内存
    device.bindBufferMemory(buffer, bufferMemory, 0);
}

void VulkanUtil::copyBuffer(
    vk::Buffer srcBuffer,
    vk::Buffer dstBuffer,
    vk::DeviceSize size)
{
    VulkanRHI* vulkanRHI = gRuntimeGlobalContext.getRHI();

    vk::CommandBuffer buffer = vulkanRHI->beginSingleTimeBuffer();

    vk::BufferCopy bufferCopy;
    bufferCopy.dstOffset = 0;
    bufferCopy.size = size;
    bufferCopy.srcOffset = 0;
    buffer.copyBuffer(srcBuffer, dstBuffer, bufferCopy);

    vulkanRHI->endSingleTimeBuffer(buffer);
}
