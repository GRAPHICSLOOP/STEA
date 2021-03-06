#pragma once
#include <memory>
#include <vulkan/vulkan.hpp>

enum class UNIFORMBUFFERTYPE
{
	UBT_Object,
	UBT_Camera,
};

struct BufferAttributes
{
public:
	BufferAttributes()
	{

	}

	BufferAttributes(UNIFORMBUFFERTYPE type, uint32_t count, uint32_t size, vk::DescriptorType descriptorType, vk::ShaderStageFlags shaderStage)
		:mType(type), mSize(size), mCount(count), mOffset(0), mDescriptorType(descriptorType), mShaderStage(shaderStage)
	{

	}

	UNIFORMBUFFERTYPE mType;
	uint32_t mSize;				// 数据的大小
	vk::DeviceSize mBufferSize;	// buffer大小
	uint32_t mOffset;			// 内存上的偏移
	uint32_t mCount;			// buffer的数量
	uint32_t mAlignments;		// 内存上的对齐大小
	vk::DescriptorType mDescriptorType;
	vk::ShaderStageFlags mShaderStage;

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

	static std::shared_ptr<BufferResource> create(vk::DescriptorSetLayout setLayout,const std::vector<BufferAttributes>& bufferAttributes);
	void* getData(UNIFORMBUFFERTYPE type);
	void getDynamicOffsets(std::vector<uint32_t>& offset,const uint32_t index);

public:
	vk::DescriptorSet mDescriptorSet;

	std::vector<BufferAttributes> mBufferAttributes;
private:
	void setUpAlignment();
	void createDescriptorSet(vk::DescriptorSetLayout setLayout);

private:

};

