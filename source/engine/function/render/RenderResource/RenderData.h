#pragma once
#include "glm/glm.hpp"
#include <vulkan/vulkan.hpp>
#include <vector>


enum DESCRIPTOR_TYPE
{
	DT_ObjectUniform = 0,
	DT_CamearUniform = 1,
	DT_Sample = 2,
	DT_PushConstantsUniform = 3,
	DT_Count = 4
};

struct VertexBufferData
{
public:
	glm::vec3 mPosition;
	glm::vec3 mNormal;
	glm::vec2 mTexCoord;
};

struct CameraBufferData
{
public:
	glm::mat4 mView;
	glm::mat4 mProj;
	glm::mat4 mViewPorj;
	glm::vec3 mLightPos;
	float mPaddingSpecularStrengthl;
	glm::vec3 mEyePos;
	float mPaddingPow;
};

struct ObjectBufferData
{
public:
	glm::mat4 mModel;
};

struct BufferData
{
public:
	BufferData()
	{

	}

	BufferData(void* data, size_t size)
		:mData(data), mSize(size)
	{

	}

	void updateData(void* data)
	{
		memcpy(mData, data, mSize);
	}

public:
	void* mData;
	size_t mSize;
};

class CameraBufferResource
{
public:
	CameraBufferResource();
	CameraBufferResource(vk::DeviceSize bufferDataSize);
	~CameraBufferResource();

public:
	void updateData(void* data);

public:
	vk::Buffer mBuffer;
	vk::DeviceMemory mMemory;
	vk::DescriptorSet mDescriptorSet;

private:
	void spawnBuffer(vk::DeviceSize bufferDataSize);
	void createDescriptorSet(vk::DeviceSize bufferDataSize);

private:
	void* mData;
	vk::DeviceSize mBufferDataSize;

};

class ObjectBufferResource
{
public:
	ObjectBufferResource();
	ObjectBufferResource(vk::DeviceSize bufferDataSize);
	~ObjectBufferResource();

public:
	vk::Buffer mBuffer;
	vk::DeviceMemory mMemory;
	vk::DescriptorSet mDescriptorSet;

public:
	void updateData(void* data);

private:
	void spawnBuffer(vk::DeviceSize bufferDataSize);
	void createDescriptorSet(vk::DeviceSize bufferDataSize);

private:
	void* mData;
	vk::DeviceSize mBufferDataSize;
};

