#pragma once
#include <memory>
#include <vulkan/vulkan.hpp>
#include "RenderResource/RenderData.h"
#include "RenderResource/MeshResource.h"
#include "RenderResource/TextureResource.h"
#include "function/render/RenderCamera.h"
#include "RenderResource/BufferResource.h"
#include "RenderResource/TextureArrayResource.h"


struct ModelRenderResource
{
public:
	std::weak_ptr<MeshResource> mMeshResource;
	std::weak_ptr<TextureResource> mTextureResource;
};

class RenderResource
{
public:
	~RenderResource();
	void initialize();
	void updatePerFrameBuffer(std::shared_ptr<RenderCamera> camera);
	void addObjectBufferResource(size_t objectID, void* data, vk::DeviceSize dataSize);
	vk::DescriptorSetLayout getDescriptorSetLayout(DESCRIPTOR_TYPE type);

public:
	CameraBufferData mCameraBufferData;
	std::unordered_map<size_t, ObjectBufferData> mObjectBufferData;
	std::unordered_map<size_t, std::vector<ModelRenderResource>> mModelRenderResources;
	std::unordered_map<size_t, std::weak_ptr<TextureResource>> mGlobalTextureResources;
	std::shared_ptr<BufferResource> mUniformResource;

private:
	void createBufferResource();
	void createDescriptorSetLayout();
	void updateUniformBuffer();

private:
	std::vector<vk::DescriptorSetLayout> mDescSetLayouts;
};

