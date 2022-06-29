#pragma once
#include "glm/glm.hpp"
#include <vulkan/vulkan.hpp>
#include <vector>


enum DESCRIPTOR_TYPE
{
	DESCRIPTOR_TYPE_OBJECTUNIFORM = 0,
	DESCRIPTOR_TYPE_CAMERAUNIFORM = 1,
	DESCRIPTOR_TYPE_SAMPLE = 2,
	DESCRIPTOR_TYPE_COUNT = 3
};

struct CameraBufferData
{
public:
	glm::mat4 mView;
	glm::mat4 mProj;
	glm::mat4 mViewPorj;
};

struct ObjectBufferData
{
public:
	glm::mat4 mModel;
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

