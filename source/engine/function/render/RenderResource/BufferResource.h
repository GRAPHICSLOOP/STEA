#pragma once
#include <memory>
#include <vulkan/vulkan.hpp>
#include "../shader/Shader.h"

struct BufferAttributes
{
public:
	BufferAttributes()
	{

	}

	BufferAttributes(uint32_t count, uint32_t size, vk::DescriptorType descriptorType, vk::ShaderStageFlags shaderStage, std::string varName)
		:mSize(size), mCount(count), mOffset(0), mDescriptorType(descriptorType), mShaderStage(shaderStage), mVarName(varName)
	{

	}

	uint32_t mSize;				// 数据的大小
	vk::DeviceSize mBufferSize;	// buffer大小
	uint32_t mOffset;			// 内存上的偏移
	uint32_t mCount;			// buffer的数量
	uint32_t mAlignments;		// 内存上的对齐大小，这个是实际大小然后符合内存的对齐大小
	uint32_t mRange;			// 一个数据的范围，这是数据实际需要的大小
	vk::DescriptorType mDescriptorType;
	vk::ShaderStageFlags mShaderStage;
	std::string mVarName;

	void* mData;
	vk::Buffer mBuffer;
	vk::DeviceMemory mBufferMemory;
};

// 管理buffer的生命周期
class BufferResource
{
private:
	BufferResource()
	{

	}

public:
	~BufferResource();

	static std::shared_ptr<BufferResource> create(Shader* shader,const std::vector<BufferAttributes>& bufferAttributes);
	void* getData(std::string type);
	void getDynamicOffsets(std::vector<uint32_t>& offset,const uint32_t index);

public:
	std::vector<BufferAttributes> mBufferAttributes;
	vk::DescriptorSet mDescriptorSet;

private:
	void setUpAlignment();
	void createDescriptorSet(Shader* shader);
};

