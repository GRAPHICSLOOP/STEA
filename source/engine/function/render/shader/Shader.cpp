#include "Shader.h"
#include "../../platform/file/FileManager.h"
#include "../../global/RuntimeGlobalContext.h"

inline Shader::Shader()
{
    mMaxSet = 0;
}

std::shared_ptr<Shader> Shader::create(const std::vector<ShaderInfo>& info)
{
    std::shared_ptr<Shader> shader = std::make_shared<Shader>();
    shader->mShaderModule.resize(info.size());
    for (uint32_t i = 0; i < info.size(); i++)
    {
        createShaderModule(info[i], &shader->mShaderModule[i]);
    }
    shader->refractionInfo();
    return shader;
}

void Shader::createShaderModule(const ShaderInfo& info, ShaderModule* module)
{
    uint8_t* data = nullptr;
    uint32_t dataSize = 0;
    FileManager::readFile(info.mFilePath, data, dataSize);

    vk::Device device = gRuntimeGlobalContext.getRHI()->mDevice;
    vk::ShaderModuleCreateInfo shaderInfo;
    shaderInfo.pCode = (uint32_t*)data;
    shaderInfo.codeSize = dataSize;
    vk::ShaderModule shaderModule = device.createShaderModule(shaderInfo);

    module->mShaderModule = shaderModule;
    module->mStage = info.mStage;
    module->mData = data;
    module->mCount = dataSize;
}

void Shader::refractionInfo()
{
    for (const auto& module : mShaderModule)
    {
        ProcessShaderModule(&module);
    }

    GenerateLayout();
}

void Shader::ProcessShaderModule(const ShaderModule* module)
{
    spirv_cross::Compiler compiler((uint32_t*)module->mData, module->mCount / (sizeof(uint32_t)));
    spirv_cross::ShaderResources resources = compiler.get_shader_resources();

    ProcessUniformBuffers(compiler, resources, module->mStage);
    ProcessProcessTextures(compiler, resources, module->mStage);
    ProcessAttachment(compiler, resources, module->mStage);
}

void Shader::ProcessUniformBuffers(spirv_cross::Compiler& compiler, spirv_cross::ShaderResources& resources, vk::ShaderStageFlags stage)
{
    for (uint32_t i = 0; i < resources.uniform_buffers.size(); i++)
    {
        spirv_cross::Resource& res = resources.uniform_buffers[i];
        spirv_cross::SPIRType type = compiler.get_type(res.type_id);
        spirv_cross::SPIRType base_type = compiler.get_type(res.base_type_id);
        const std::string& varName = compiler.get_name(res.id);
        const std::string& typeName = compiler.get_name(res.base_type_id);
        uint32_t uniformBufferStructSize = (uint32_t)compiler.get_declared_struct_size(type);

        uint32_t set = compiler.get_decoration(res.id, spv::Decoration::DecorationDescriptorSet);
        uint32_t bindingIndex = compiler.get_decoration(res.id, spv::Decoration::DecorationBinding);

        // [layout(set = 0 , binding = 1) uniform ObjectDynamicBuffer] 当名称有Dynamic关键字时，认为是eUniformBufferDynamic
        vk::DescriptorSetLayoutBinding binding;
        binding.binding = bindingIndex;
        binding.descriptorCount = 1;
        binding.descriptorType = typeName.find("Dynamic") != std::string::npos ? vk::DescriptorType::eUniformBufferDynamic : vk::DescriptorType::eUniformBuffer;
        binding.stageFlags = stage;

        addSetLayoutBinding(set, typeName, binding);
    }
}

void Shader::ProcessProcessTextures(spirv_cross::Compiler& compiler, spirv_cross::ShaderResources& resources, vk::ShaderStageFlags stage)
{
    for (uint32_t i = 0; i < resources.sampled_images.size(); i++)
    {
        spirv_cross::Resource& res = resources.sampled_images[i];
        const std::string& varName = compiler.get_name(res.id);

        uint32_t set = compiler.get_decoration(res.id, spv::Decoration::DecorationDescriptorSet);
        uint32_t bindingIndex = compiler.get_decoration(res.id, spv::Decoration::DecorationBinding);

        vk::DescriptorSetLayoutBinding binding;
        binding.binding = bindingIndex;
        binding.descriptorCount = 1;
        binding.descriptorType = vk::DescriptorType::eCombinedImageSampler;
        binding.stageFlags = stage;

        addSetLayoutBinding(set, varName, binding);
    }
}

void Shader::ProcessAttachment(spirv_cross::Compiler& compiler, spirv_cross::ShaderResources& resources, vk::ShaderStageFlags stage)
{
    for (uint32_t i = 0; i < resources.subpass_inputs.size(); i++)
    {
        spirv_cross::Resource& res = resources.subpass_inputs[i];
        const std::string& varName = compiler.get_name(res.id);

        uint32_t set = compiler.get_decoration(res.id, spv::Decoration::DecorationDescriptorSet);
        uint32_t bindingIndex = compiler.get_decoration(res.id, spv::Decoration::DecorationBinding);

        vk::DescriptorSetLayoutBinding binding;
        binding.binding = bindingIndex;
        binding.descriptorCount = 1;
        binding.descriptorType = vk::DescriptorType::eInputAttachment;
        binding.stageFlags = stage;

        addSetLayoutBinding(set, varName, binding);
    }
}

void Shader::addSetLayoutBinding(uint32_t set, const std::string& typeName, vk::DescriptorSetLayoutBinding binding)
{
    auto map = mBindgMap.find(typeName);
    if (map == mBindgMap.end())
    {
        mBindgMap[typeName].mBinding = binding;
        mBindgMap[typeName].mSet = set;
    }
    else
    {
        // 添加多一个stage
        map->second.mBinding.stageFlags |= binding.stageFlags;
    }

    mMaxSet = set > mMaxSet ? set : mMaxSet;
}

void Shader::GenerateLayout()
{
    for (uint32_t i = 0; i <= mMaxSet; i++)
    {
        std::vector<vk::DescriptorSetLayoutBinding> binding;
        for (const auto& bind : mBindgMap)
        {
            if (bind.second.mSet == i)
            {
                binding.push_back(bind.second.mBinding);
            }
        }

        vk::DescriptorSetLayoutCreateInfo info;
        info.bindingCount = (uint32_t)binding.size();
        info.pBindings = binding.data();
        mDescriptorSetLayouts.push_back(gRuntimeGlobalContext.getRHI()->mDevice.createDescriptorSetLayout(info));
    }
}

