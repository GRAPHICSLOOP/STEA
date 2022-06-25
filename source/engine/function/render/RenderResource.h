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
	void createVertexBuffer(struct MeshBufferResource& bufferResouce, const void* VerticesData, uint32_t count);
	void createIndexBuffer(struct MeshBufferResource& bufferResouce, const void* indicesData, uint32_t count);
	void updatePerFrameBuffer(std::shared_ptr<RenderCamera> camera);
	void addObjectBufferResource(size_t objectID, void* data, vk::DeviceSize dataSize);
	vk::DescriptorSetLayout getDescriptorSetLayout(DESCRIPTOR_TYPE type);

public:
	CameraBufferResource mCameraBufferResource;
	std::unordered_map<size_t, std::shared_ptr<ObjectBufferResource>> mObjectBufferResources;
	std::unordered_map<size_t, std::vector<ModelRenderResource>> mModelRenderResources;
	std::unordered_map<size_t, std::weak_ptr<TextureResource>> mGlobalTextureResources;

private:
	void createBufferResource();
	void createDescriptorSetLayout();

private:
	std::array<vk::DescriptorSetLayout, 2> mDescSetLayouts;
};

