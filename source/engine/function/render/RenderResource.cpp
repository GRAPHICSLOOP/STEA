#include "RenderResource.h"
#include <glm/gtc/matrix_transform.hpp>
#include "VulkanUtil.h"
#include "core/base/macro.h"
#include "function/global/RuntimeGlobalContext.h"
#include "glm/gtc/random.hpp"

RenderResource::~RenderResource()
{

}

void RenderResource::initialize()
{
    createShaders();
    createLight();
    createBufferResource();
}

void RenderResource::updatePerFrameBuffer(float deltaTime , std::shared_ptr<RenderCamera> camera)
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

    updateUniformBuffer(deltaTime);
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
        BufferAttributes(1, sizeof(CameraBufferData), vk::DescriptorType::eUniformBuffer, vk::ShaderStageFlagBits::eVertex,"CameraBuffer"),
        BufferAttributes(16, sizeof(ObjectBufferData), vk::DescriptorType::eUniformBufferDynamic, vk::ShaderStageFlagBits::eVertex,"ObjectDynamicBuffer"),
        });

    mLightUniformResource = BufferResource::create(getShader("light"), {
        BufferAttributes(1, sizeof(CameraBufferData), vk::DescriptorType::eUniformBuffer, vk::ShaderStageFlagBits::eVertex | vk::ShaderStageFlagBits::eFragment,"CameraBuffer"),
        BufferAttributes(LIGHT_MAXNUMB, sizeof(LightBufferData), vk::DescriptorType::eUniformBuffer, vk::ShaderStageFlagBits::eVertex,"LightBuffer"),
    });

    mQuadUniformResource = BufferResource::create(getShader("quad"), {
        BufferAttributes(1, sizeof(CameraBufferData), vk::DescriptorType::eUniformBuffer, vk::ShaderStageFlagBits::eVertex,"CameraBuffer"),
        BufferAttributes(LIGHT_MAXNUMB, sizeof(LightBufferData), vk::DescriptorType::eUniformBuffer, vk::ShaderStageFlagBits::eFragment,"LightBuffer"),
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

    std::shared_ptr<Shader> lightShader = Shader::create
    ({
        ShaderInfo("shaders/light.vspv",vk::ShaderStageFlagBits::eVertex),
        ShaderInfo("shaders/light.fspv",vk::ShaderStageFlagBits::eFragment)
    });

    mGlobalShader["quad"] = quadShader;
    mGlobalShader["obj"] = objShader;
    mGlobalShader["light"] = lightShader;
}

void RenderResource::createLight()
{
    BoundingBox bounds(glm::vec3(0.f), glm::vec3(5.f,2.f,5.f));
    for (uint32_t i = 0; i < LIGHT_MAXNUMB; i++)
    {
        mLightDatas[i].mPosition.x = glm::linearRand(bounds.mMin.x, bounds.mMax.x);
        mLightDatas[i].mPosition.y = glm::linearRand(bounds.mMin.y, bounds.mMax.y);
        mLightDatas[i].mPosition.z = glm::linearRand(bounds.mMin.z, bounds.mMax.z);

        mLightDatas[i].mColor.x = glm::linearRand(0.0f, 1.0f);
        mLightDatas[i].mColor.y = glm::linearRand(0.0f, 1.0f);
        mLightDatas[i].mColor.z = glm::linearRand(0.0f, 1.0f);

        mLightDatas[i].mRadius = glm::linearRand(0.1f, 3.5f);

        mLightInfos[i].mPosition = mLightDatas[i].mPosition;
        mLightInfos[i].mDirection = glm::normalize(mLightDatas[i].mPosition);
        mLightInfos[i].mSpeed = 1.0f + glm::linearRand(0.0f, 5.0f);
    }
}

void RenderResource::updateUniformBuffer(float deltaTime)
{
    // 更新cameraData
    void* cameraData = mUniformResource->getData("CameraBuffer");
    memcpy(cameraData, &mCameraBufferData, sizeof(CameraBufferData));
    
    // 更新objectData
    std::vector<ObjectBufferData> objectData(mObjectBufferData.size());
    uint32_t count = 0;
    for (const auto& iter : mObjectBufferData)
    {
        objectData[count] = iter.second;
        count++;
    }

    void* bufferData = mUniformResource->getData("ObjectDynamicBuffer");
    memcpy(bufferData, objectData.data(), sizeof(ObjectBufferData) * mObjectBufferData.size());

    // 更新lightData
    for (uint32_t i = 0; i < LIGHT_MAXNUMB; i++)
    {
        float bias = glm::sin(deltaTime * mLightInfos[i].mSpeed) / 5.0f;
        mLightDatas[i].mPosition.x = mLightInfos[i].mPosition.x + bias * mLightInfos[i].mDirection.x * 10.0f;
        mLightDatas[i].mPosition.y = mLightInfos[i].mPosition.y + bias * mLightInfos[i].mDirection.y * 10.0f;
        mLightDatas[i].mPosition.z = mLightInfos[i].mPosition.z + bias * mLightInfos[i].mDirection.z * 10.0f;
    }

    void* lightData;

    // 更新lightcameraData
    cameraData = mLightUniformResource->getData("CameraBuffer");
    memcpy(cameraData, &mCameraBufferData, sizeof(CameraBufferData));
    lightData = mLightUniformResource->getData("LightBuffer");
    memcpy(lightData, mLightDatas.data(), sizeof(mLightDatas));

    // 更新quadData
    lightData = mQuadUniformResource->getData("LightBuffer");
    memcpy(lightData, mLightDatas.data(), sizeof(mLightDatas));
}
