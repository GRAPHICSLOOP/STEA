#pragma once
#include <memory>
#include <vulkan/vulkan.hpp>
#include "RenderResource/RenderData.h"
#include "RenderResource/MeshResource.h"
#include "RenderResource/TextureResource.h"
#include "function/render/RenderCamera.h"

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
	std::shared_ptr<CameraBufferResource> mCameraBufferResource;
	std::unordered_map<size_t, std::shared_ptr<ObjectBufferResource>> mObjectBufferResources;
	std::unordered_map<size_t, std::vector<ModelRenderResource>> mModelRenderResources;
	std::unordered_map<size_t, std::weak_ptr<TextureResource>> mGlobalTextureResources;

private:
	void createBufferResource();
	void createDescriptorSetLayout();

private:
	std::vector<vk::DescriptorSetLayout> mDescSetLayouts;
};

