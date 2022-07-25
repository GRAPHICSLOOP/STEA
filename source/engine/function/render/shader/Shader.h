#pragma once
#include <vector>
#include <vulkan/vulkan.hpp>
#include <spirv_cross.hpp>

struct ShaderInfo
{
	ShaderInfo(const char* filePath, vk::ShaderStageFlagBits stage)
		:mFilePath(filePath),mStage(stage)
	{

	}

	const char* mFilePath;
	vk::ShaderStageFlagBits mStage;
};

struct ShaderModule
{
	~ShaderModule()
	{
		delete[] mData;
	}

	vk::ShaderStageFlagBits mStage;
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
	void updateDescriptorSet(const std::string& varName, vk::DescriptorSet descriptorSet, const vk::DescriptorImageInfo* imageInfo, const vk::DescriptorBufferInfo* bufferInfo);
	const DescriptorSetLayoutInfo* getBindding(std::string varName);
	std::vector<vk::DescriptorSet> generateSet(const std::vector<std::string>& varNames);

public:
	std::vector<vk::DescriptorSetLayout> mDescriptorSetLayouts;
	std::vector<ShaderModule> mShaderModule;

private:
	void refractionInfo();
	void ProcessShaderModule(const ShaderModule* module);
	void ProcessUniformBuffers(spirv_cross::Compiler& compiler, spirv_cross::ShaderResources& resources, vk::ShaderStageFlags stage);
	void ProcessProcessTextures(spirv_cross::Compiler& compiler, spirv_cross::ShaderResources& resources, vk::ShaderStageFlags stage);
	void ProcessAttachment(spirv_cross::Compiler& compiler, spirv_cross::ShaderResources& resources, vk::ShaderStageFlags stage);
	void addSetLayoutBinding(uint32_t set, const std::string& typeName, vk::DescriptorSetLayoutBinding binding);

	void generateLayout();
	
private:
	std::unordered_map<std::string, DescriptorSetLayoutInfo> mBindgMap;
	uint32_t mMaxSet;
};

