#pragma once
#include <vulkan/vulkan.hpp>
#include <memory>

class IndexResource
{
private:
	IndexResource();

public:
	~IndexResource();

public:
	uint32_t mIndexCount;
	vk::Buffer mBuffer;
	vk::DeviceMemory mBufferMemory;

public:
	static std::shared_ptr<IndexResource> create(const void* indicesData, uint32_t count);
};

