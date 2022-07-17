#pragma once
#include <memory>
#include <vulkan/vulkan.hpp>
#include "RenderResource/RenderData.h"
#include "RenderResource/MeshResource.h"
#include "RenderResource/ImageResource.h"
#include "function/render/RenderCamera.h"
#include "RenderResource/BufferResource.h"
#include "RenderResource/TextureArrayResource.h"
#include "shader/Shader.h"


struct ModelRenderResource
{
public:
	std::weak_ptr<MeshResource> mMeshResource;
	std::weak_ptr<ImageResource> mTextureResource;
};

class RenderResource
{
public:
	~RenderResource();
	void initialize();
	void updatePerFrameBuffer(std::shared_ptr<RenderCamera> camera);
	void addObjectBufferResource(size_t objectID, void* data, vk::DeviceSize dataSize);
	//vk::DescriptorSetLayout getDescriptorSetLayout(DESCRIPTOR_TYPE type);
	std::vector<vk::DescriptorSetLayout> getDescriptorSetLayout(std::string shaderName);

public:
	CameraBufferData mCameraBufferData;
	std::unordered_map<size_t, ObjectBufferData> mObjectBufferData;
	std::unordered_map<size_t, std::vector<ModelRenderResource>> mModelRenderResources;
	std::unordered_map<size_t, std::weak_ptr<ImageResource>> mGlobalTextureResources;
	std::shared_ptr<BufferResource> mUniformResource;

private:
	void createBufferResource();
	void createShaders();
	void createDescriptorSetLayout();
	void updateUniformBuffer();

private:
	std::vector<vk::DescriptorSetLayout> mDescSetLayouts;
	std::unordered_map<std::string, std::shared_ptr<Shader>> mGlobalShader;
};

