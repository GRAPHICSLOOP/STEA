#pragma once
#include <vector>
#include <vulkan/vulkan.hpp>
#include <spirv_cross.hpp>

struct ShaderInfo
{
	ShaderInfo(const char* filePath, vk::ShaderStageFlags stage)
		:mFilePath(filePath),mStage(stage)
	{

	}

	const char* mFilePath;
	vk::ShaderStageFlags mStage;
};

struct ShaderModule
{
	~ShaderModule()
	{
		delete[] mData;
	}

	vk::ShaderStageFlags mStage;
	vk::ShaderModule mShaderModule;
	uint8_t* mData;
	uint32_t mCount;


};

struct DescriptorSetLayoutInfo
{
	uint32_t mSet;
	vk::DescriptorSetLayoutBinding mBinding;
};

class Shader
{
public:
	Shader();
	static std::shared_ptr<Shader> create(const std::vector<ShaderInfo>& info);
	static void createShaderModule(const ShaderInfo& info,ShaderModule* module);
	
public:
	std::vector<vk::DescriptorSetLayout> mDescriptorSetLayouts;

private:
	void refractionInfo();
	void ProcessShaderModule(const ShaderModule* module);
	void ProcessUniformBuffers(spirv_cross::Compiler& compiler, spirv_cross::ShaderResources& resources, vk::ShaderStageFlags stage);
	void ProcessProcessTextures(spirv_cross::Compiler& compiler, spirv_cross::ShaderResources& resources, vk::ShaderStageFlags stage);
	void ProcessAttachment(spirv_cross::Compiler& compiler, spirv_cross::ShaderResources& resources, vk::ShaderStageFlags stage);
	void addSetLayoutBinding(uint32_t set, const std::string& typeName, vk::DescriptorSetLayoutBinding binding);

	void GenerateLayout();

private:
	std::unordered_map<std::string, DescriptorSetLayoutInfo> mBindgMap;
	uint32_t mMaxSet;
	std::vector<ShaderModule> mShaderModule;
};

