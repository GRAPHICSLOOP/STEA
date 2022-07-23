#pragma once
#include <vulkan/vulkan.hpp>
#include "RenderResource/ImageResource.h"

class VulkanUtil
{
public:
    static uint32_t findMemoryType(
        uint32_t typeFilter,
        vk::MemoryPropertyFlags property);

    static void createBuffer(
        vk::DeviceSize size,
        vk::BufferUsageFlags usage,
        vk::MemoryPropertyFlags properties,
        vk::Buffer& buffer,
        vk::DeviceMemory& bufferMemory);

    static void copyBuffer(
        vk::Buffer srcBuffer,
        vk::Buffer dstBuffer,
        vk::DeviceSize size);

};

