#include<fstream>
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

std::vector<char> VulkanUtil::readFile(const char* fileName)
{
    std::ifstream file(fileName, std::ios::ate | std::ios::binary); // 末尾且二进制读取

    if (!file.is_open())
    {
        throw std::runtime_error("open file error");
    }

    size_t size = file.tellg();
    std::vector<char> buffer(size);
    file.seekg(0); // 回到头部
    file.read(buffer.data(), size);

    file.close();
    return buffer;
}

vk::ShaderModule VulkanUtil::createShaderModule(const std::vector<char>& code)
{
    vk::Device device = gRuntimeGlobalContext.getRHI()->mDevice;

    vk::ShaderModuleCreateInfo shaderInfo;
    shaderInfo.pCode = reinterpret_cast<const uint32_t*>(code.data());
    shaderInfo.codeSize = code.size();

    vk::ShaderModule shaderModule = device.createShaderModule(shaderInfo);
    CHECK_NULL(shaderModule);
    return shaderModule;
}

vk::ShaderModule VulkanUtil::loadShaderModuleFromFile(const char* fileName)
{
    std::vector<char> code = readFile(fileName);
    return createShaderModule(code);
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
