#include "RenderResource.h"
#include <glm/gtc/matrix_transform.hpp>
#include "VulkanUtil.h"
#include "core/base/macro.h"
#include "function/global/RuntimeGlobalContext.h"

RenderResource::~RenderResource()
{

}

void RenderResource::initialize()
{
    createShaders();
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

Material* RenderResource::createMaterial(std::string name, Shader* shader, const std::vector<MaterialAttribute>& attribute)
{
    auto iter = mGlobalMaterials.find(name);
    if (iter != mGlobalMaterials.end())
        return iter->second.get();

    mGlobalMaterials[name] = std::make_shared<Material>(name,shader, attribute);
    return mGlobalMaterials[name].get();
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

Shader* RenderResource::getShader(std::string shaderName)
{
    auto iter = mGlobalShader.find(shaderName);
    if (iter == mGlobalShader.end())
    {
        STEALOG_ERROR("null shader {} ", shaderName.c_str());
        return nullptr;
    }
    else
    {
        return iter->second.get();
    }
}

void RenderResource::createBufferResource()
{
    mUniformResource = BufferResource::create(getShader("obj"), {
        BufferAttributes(UNIFORMBUFFERTYPE::UBT_Camera, 1, sizeof(CameraBufferData), vk::DescriptorType::eUniformBuffer, vk::ShaderStageFlagBits::eVertex | vk::ShaderStageFlagBits::eFragment,"CameraBuffer"),
        BufferAttributes(UNIFORMBUFFERTYPE::UBT_Object, 16, sizeof(ObjectBufferData), vk::DescriptorType::eUniformBufferDynamic, vk::ShaderStageFlagBits::eVertex,"ObjectDynamicBuffer")
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

void RenderResource::updateUniformBuffer()
{
    void* cameraData = mUniformResource->getData(UNIFORMBUFFERTYPE::UBT_Camera);
    memcpy(cameraData, &mCameraBufferData, sizeof(CameraBufferData));
    
    std::vector<ObjectBufferData> objectData(mObjectBufferData.size());
    uint32_t count = 0;
    for (const auto& iter : mObjectBufferData)
    {
        objectData[count] = iter.second;
        count++;
    }

    void* bufferData = mUniformResource->getData(UNIFORMBUFFERTYPE::UBT_Object);
    memcpy(bufferData, objectData.data(), sizeof(ObjectBufferData) * mObjectBufferData.size());
}
