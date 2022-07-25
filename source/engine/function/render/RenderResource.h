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
#include "shader/Material.h"

struct ModelRenderResource
{
public:
	MeshResource* mMeshResource;
	Material* mMaterial;
};

class RenderResource
{
public:
	~RenderResource();
	void initialize();
	void updatePerFrameBuffer(std::shared_ptr<RenderCamera> camera);
	void addObjectBufferResource(size_t objectID, void* data, vk::DeviceSize dataSize);
	Material* createMaterial(std::string name, Shader* shader, const std::vector<MaterialAttribute>& attribute);
	std::vector<vk::DescriptorSetLayout> getDescriptorSetLayout(std::string shaderName);
	Shader* getShader(std::string shaderName);

public:
	CameraBufferData mCameraBufferData;
	std::unordered_map<size_t, ObjectBufferData> mObjectBufferData;
	std::unordered_map<size_t, std::vector<ModelRenderResource>> mModelRenderResources;
	std::unordered_map<size_t, ImageResource*> mGlobalTextureResources;
	std::shared_ptr<BufferResource> mUniformResource;

private:
	void createBufferResource();
	void createShaders();
	void updateUniformBuffer();

private:
	std::unordered_map<std::string, std::shared_ptr<Shader>> mGlobalShader;
	std::unordered_map<std::string, std::shared_ptr<Material>> mGlobalMaterials;
};

