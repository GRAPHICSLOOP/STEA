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
    createDescriptorSetLayout();
    createBufferResource();
}

void RenderResource::updatePerFrameBuffer(std::shared_ptr<RenderCamera> camera)
{
    CameraBufferData data;
    data.mView = camera->getViewMatrix();
    data.mProj = glm::perspectiveRH(glm::radians(45.f),
        gRuntimeGlobalContext.getRHI()->mSwapchainSupportDetails.mExtent2D.width / (float)gRuntimeGlobalContext.getRHI()->mSwapchainSupportDetails.mExtent2D.height,
        0.1f,
        100.f);
    data.mProj[1][1] *= -1;
    data.mViewPorj = data.mProj * data.mView;
    data.mLightPos = glm::vec3(1.f);
    data.mEyePos = camera->getPosition();
    data.mPaddingPow = 32.f;
    data.mPaddingSpecularStrengthl = 0.5f;

    mCameraBufferResource->updateData(&data);
}

void RenderResource::addObjectBufferResource(size_t objectID, void* data, vk::DeviceSize dataSize)
{
    auto iter = mObjectBufferResources.find(objectID);
    if (iter == mObjectBufferResources.end())
    {
        std::shared_ptr<ObjectBufferResource> resource = std::make_shared<ObjectBufferResource>(dataSize);
        resource->updateData(data);
        mObjectBufferResources[objectID] = resource;
        return;
    }

    iter->second->updateData(data);
}

vk::DescriptorSetLayout RenderResource::getDescriptorSetLayout(DESCRIPTOR_TYPE type)
{
    CHECK_NULL(mDescSetLayouts[type]);
    return mDescSetLayouts[type];
}

void RenderResource::createBufferResource()
{
    mCameraBufferResource =std::make_shared<CameraBufferResource>(sizeof(CameraBufferData));
}

void RenderResource::createDescriptorSetLayout()
{
    vk::DescriptorSetLayoutCreateInfo info;

    // mesh固定的uniform DescriptorSetLayout
    vk::DescriptorSetLayoutBinding objectUniformBinding;
    objectUniformBinding.binding = 0;
    objectUniformBinding.descriptorCount = 1;
    objectUniformBinding.descriptorType = vk::DescriptorType::eUniformBuffer;
    objectUniformBinding.stageFlags = vk::ShaderStageFlagBits::eVertex;
    info.bindingCount = 1;
    info.pBindings = &objectUniformBinding;
    mDescSetLayouts[DESCRIPTOR_TYPE::DESCRIPTOR_TYPE_OBJECTUNIFORM] = gRuntimeGlobalContext.getRHI()->mDevice.createDescriptorSetLayout(info);

    vk::DescriptorSetLayoutBinding cameraUniformBinding;
    cameraUniformBinding.binding = 0;
    cameraUniformBinding.descriptorCount = 1;
    cameraUniformBinding.descriptorType = vk::DescriptorType::eUniformBuffer;
    cameraUniformBinding.stageFlags = vk::ShaderStageFlagBits::eVertex | vk::ShaderStageFlagBits::eFragment;
    info.bindingCount = 1;
    info.pBindings = &cameraUniformBinding;
    mDescSetLayouts[DESCRIPTOR_TYPE::DESCRIPTOR_TYPE_CAMERAUNIFORM] = gRuntimeGlobalContext.getRHI()->mDevice.createDescriptorSetLayout(info);

    // 变动较多的 sample DescriptorSetLayout
    vk::DescriptorSetLayoutBinding sampleBinding;
    sampleBinding.binding = 0;
    sampleBinding.descriptorCount = 1;
    sampleBinding.descriptorType = vk::DescriptorType::eCombinedImageSampler;
    sampleBinding.stageFlags = vk::ShaderStageFlagBits::eFragment;
    info.bindingCount = 1;
    info.pBindings = &sampleBinding;
    mDescSetLayouts[DESCRIPTOR_TYPE::DESCRIPTOR_TYPE_SAMPLE] = gRuntimeGlobalContext.getRHI()->mDevice.createDescriptorSetLayout(info);
}
