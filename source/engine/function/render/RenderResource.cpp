#include "RenderResource.h"
#include <glm/gtc/matrix_transform.hpp>
#include "VulkanUtil.h"
#include "core/base/macro.h"
#include "function/global/RuntimeGlobalContext.h"

RenderResource::~RenderResource()
{
    for (uint32_t i = 0; i < mDescSetLayouts.size(); i++)
    {
        gRuntimeGlobalContext.getRHI()->mDevice.destroyDescriptorSetLayout(mDescSetLayouts[i]);
    }
}

void RenderResource::initialize()
{
    createShaders();
    createDescriptorSetLayout();
    createBufferResource();
}

void RenderResource::updatePerFrameBuffer(std::shared_ptr<RenderCamera> camera)
{
    mCameraBufferData.mView = camera->getViewMatrix();
    mCameraBufferData.mProj = glm::perspectiveRH(glm::radians(45.f),
        gRuntimeGlobalContext.getRHI()->mSwapchainSupportDetails.mExtent2D.width / (float)gRuntimeGlobalContext.getRHI()->mSwapchainSupportDetails.mExtent2D.height,
        0.1f,
        100.f);
    mCameraBufferData.mProj[1][1] *= -1;
    mCameraBufferData.mViewPorj = mCameraBufferData.mProj * mCameraBufferData.mView;
    mCameraBufferData.mLightPos = glm::vec3(1.f);
    mCameraBufferData.mEyePos = camera->getPosition();
    mCameraBufferData.mPaddingPow = 32.f;
    mCameraBufferData.mPaddingSpecularStrengthl = 0.5f;

    updateUniformBuffer();
}

void RenderResource::addObjectBufferResource(size_t objectID, void* data, vk::DeviceSize dataSize)
{
    auto iter = mObjectBufferData.find(objectID);
    if (iter == mObjectBufferData.end())
    {
        mObjectBufferData[objectID] = *(ObjectBufferData*)data;
        return;
    }

    mObjectBufferData[objectID] = *(ObjectBufferData*)data;
}

//vk::DescriptorSetLayout RenderResource::getDescriptorSetLayout(DESCRIPTOR_TYPE type)
//{
//    CHECK_NULL(mDescSetLayouts[type]);
//    return mDescSetLayouts[type];
//}

std::vector<vk::DescriptorSetLayout> RenderResource::getDescriptorSetLayout(std::string shaderName)
{
    auto iter = mGlobalShader.find(shaderName);
    if (iter == mGlobalShader.end())
    {
        STEALOG_ERROR("null shader {} ", shaderName.c_str());
        return std::vector<vk::DescriptorSetLayout>();
    }
    else
    {
        return iter->second->mDescriptorSetLayouts;
    }
}

void RenderResource::createBufferResource()
{
    mUniformResource = BufferResource::create(getDescriptorSetLayout("obj")[0], {
        BufferAttributes(UNIFORMBUFFERTYPE::UBT_Camera, 1, sizeof(CameraBufferData), vk::DescriptorType::eUniformBuffer, vk::ShaderStageFlagBits::eVertex | vk::ShaderStageFlagBits::eFragment),
        BufferAttributes(UNIFORMBUFFERTYPE::UBT_Object, 2, sizeof(ObjectBufferData), vk::DescriptorType::eUniformBufferDynamic, vk::ShaderStageFlagBits::eVertex)
        });
}

void RenderResource::createShaders()
{
    std::shared_ptr<Shader> quadShader = Shader::create
    ({
        ShaderInfo("shaders/quad.vspv",vk::ShaderStageFlagBits::eVertex),
        ShaderInfo("shaders/quad.fspv",vk::ShaderStageFlagBits::eFragment)
    });
    std::shared_ptr<Shader> objShader = Shader::create
    ({
        ShaderInfo("shaders/obj.vspv",vk::ShaderStageFlagBits::eVertex),
        ShaderInfo("shaders/obj.fspv",vk::ShaderStageFlagBits::eFragment)
    });

    mGlobalShader["quad"] = quadShader;
    mGlobalShader["obj"] = objShader;
}

void RenderResource::createDescriptorSetLayout()
{
    mDescSetLayouts.resize(DESCRIPTOR_TYPE::DT_Count);
    vk::DescriptorSetLayoutCreateInfo info;

    // mesh固定的uniform DescriptorSetLayout
    vk::DescriptorSetLayoutBinding objectUniformBinding;
    objectUniformBinding.binding = 0;
    objectUniformBinding.descriptorCount = 1;
    objectUniformBinding.descriptorType = vk::DescriptorType::eUniformBuffer;
    objectUniformBinding.stageFlags = vk::ShaderStageFlagBits::eVertex;
    info.bindingCount = 1;
    info.pBindings = &objectUniformBinding;
    mDescSetLayouts[DESCRIPTOR_TYPE::DT_ObjectUniform] = gRuntimeGlobalContext.getRHI()->mDevice.createDescriptorSetLayout(info);

    // 相机观察中不变的buffer
    vk::DescriptorSetLayoutBinding cameraUniformBinding;
    cameraUniformBinding.binding = 0;
    cameraUniformBinding.descriptorCount = 1;
    cameraUniformBinding.descriptorType = vk::DescriptorType::eUniformBuffer;
    cameraUniformBinding.stageFlags = vk::ShaderStageFlagBits::eVertex | vk::ShaderStageFlagBits::eFragment;
    info.bindingCount = 1;
    info.pBindings = &cameraUniformBinding;
    mDescSetLayouts[DESCRIPTOR_TYPE::DT_CamearUniform] = gRuntimeGlobalContext.getRHI()->mDevice.createDescriptorSetLayout(info);

    // 变动较多的 sample DescriptorSetLayout
    vk::DescriptorSetLayoutBinding sampleBinding;
    sampleBinding.binding = 0;
    sampleBinding.descriptorCount = 1;
    sampleBinding.descriptorType = vk::DescriptorType::eCombinedImageSampler;
    sampleBinding.stageFlags = vk::ShaderStageFlagBits::eFragment;
    info.bindingCount = 1;
    info.pBindings = &sampleBinding;
    mDescSetLayouts[DESCRIPTOR_TYPE::DT_Sample] = gRuntimeGlobalContext.getRHI()->mDevice.createDescriptorSetLayout(info);

}

void RenderResource::updateUniformBuffer()
{
    void* cameraData = mUniformResource->getData(UNIFORMBUFFERTYPE::UBT_Camera);
    memcpy(cameraData, &mCameraBufferData, sizeof(CameraBufferData));
    
    
    ObjectBufferData* bufferData = (ObjectBufferData*)mUniformResource->getData(UNIFORMBUFFERTYPE::UBT_Object);
    uint32_t count = 0;
    for (const auto& iter : mObjectBufferData)
    {
        bufferData += count;
        *bufferData = mObjectBufferData[iter.first];
        count++;
    }
}
