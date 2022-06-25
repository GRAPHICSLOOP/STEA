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

    vkUnmapMemory(gRuntimeGlobalContext.getRHI()->mDevice, mCameraBufferResource.mMemory);
    gRuntimeGlobalContext.getRHI()->mDevice.destroyBuffer(mCameraBufferResource.mBuffer);
    gRuntimeGlobalContext.getRHI()->mDevice.freeMemory(mCameraBufferResource.mMemory);
}

void RenderResource::initialize()
{
    createBufferResource();
    createDescriptorSetLayout();
}

void RenderResource::createVertexBuffer(MeshBufferResource& bufferResouce, const void* VerticesData, uint32_t count)
{
    bufferResouce.mVertexCount = count;
    size_t size = count * sizeof(VertexBufferData);

    vk::Buffer stagingBuffer;
    vk::DeviceMemory stagingBufferMemory;
    VulkanUtil::createBuffer(
        size,
        vk::BufferUsageFlagBits::eTransferSrc,
        vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostCoherent,
        stagingBuffer,
        stagingBufferMemory);


    void* data;
    vkMapMemory(gRuntimeGlobalContext.getRHI()->mDevice, stagingBufferMemory, 0, size, 0, &data);
    memcpy(data, VerticesData, size);
    vkUnmapMemory(gRuntimeGlobalContext.getRHI()->mDevice, stagingBufferMemory);

    VulkanUtil::createBuffer(
        size,
        vk::BufferUsageFlagBits::eTransferDst | vk::BufferUsageFlagBits::eVertexBuffer,
        vk::MemoryPropertyFlagBits::eDeviceLocal,
        bufferResouce.mVertexBuffer,
        bufferResouce.mVertexBufferMemory);

    VulkanUtil::copyBuffer(stagingBuffer, bufferResouce.mVertexBuffer, size);

    gRuntimeGlobalContext.getRHI()->mDevice.destroyBuffer(stagingBuffer);
    gRuntimeGlobalContext.getRHI()->mDevice.freeMemory(stagingBufferMemory);
}

void RenderResource::createIndexBuffer(MeshBufferResource& bufferResouce, const void* indicesData, uint32_t count)
{
    bufferResouce.mIndexCount = count;
    size_t size = count * sizeof(uint32_t);

    vk::Buffer stagingBuffer;
    vk::DeviceMemory stagingBufferMemory;
    VulkanUtil::createBuffer(
        size,
        vk::BufferUsageFlagBits::eTransferSrc,
        vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostCoherent,
        stagingBuffer,
        stagingBufferMemory);


    void* data;
    vkMapMemory(gRuntimeGlobalContext.getRHI()->mDevice, stagingBufferMemory, 0, size, 0, &data);
    memcpy(data, indicesData, size);
    vkUnmapMemory(gRuntimeGlobalContext.getRHI()->mDevice, stagingBufferMemory);

    VulkanUtil::createBuffer(
        size,
        vk::BufferUsageFlagBits::eTransferDst | vk::BufferUsageFlagBits::eIndexBuffer,
        vk::MemoryPropertyFlagBits::eDeviceLocal,
        bufferResouce.mIndexBuffer,
        bufferResouce.mIndexBufferMemory);

    VulkanUtil::copyBuffer(stagingBuffer, bufferResouce.mIndexBuffer, size);

    gRuntimeGlobalContext.getRHI()->mDevice.destroyBuffer(stagingBuffer);
    gRuntimeGlobalContext.getRHI()->mDevice.freeMemory(stagingBufferMemory);
}

void RenderResource::updatePerFrameBuffer(std::shared_ptr<RenderCamera> camera)
{
    (*(CameraBufferData*)mCameraBufferResource.mData).mView = camera->getViewMatrix();
    (*(CameraBufferData*)mCameraBufferResource.mData).mProj = glm::perspectiveRH(glm::radians(45.f), gRuntimeGlobalContext.getRHI()->mSwapchainSupportDetails.mExtent2D.width / (float)gRuntimeGlobalContext.getRHI()->mSwapchainSupportDetails.mExtent2D.height, 0.1f, 10.f);
    (*(CameraBufferData*)mCameraBufferResource.mData).mProj[1][1] *= -1;
    (*(CameraBufferData*)mCameraBufferResource.mData).mViewPorj = (*(CameraBufferData*)mCameraBufferResource.mData).mProj * (*(CameraBufferData*)mCameraBufferResource.mData).mView;
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
    vk::DeviceSize bufferSize = sizeof(ObjectBufferData);

    bufferSize = sizeof(CameraBufferData);
    VulkanUtil::createBuffer(
        bufferSize,
        vk::BufferUsageFlagBits::eUniformBuffer,
        vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostCoherent,
        mCameraBufferResource.mBuffer,
        mCameraBufferResource.mMemory
    );

    // 不立马解除映射 销毁RenderResource的时候才解除
    vkMapMemory(gRuntimeGlobalContext.getRHI()->mDevice, mCameraBufferResource.mMemory, 0, sizeof(CameraBufferData), 0, &mCameraBufferResource.mData);
}

void RenderResource::createDescriptorSetLayout()
{
    vk::DescriptorSetLayoutCreateInfo info;

    // mesh固定的uniform DescriptorSetLayout
    std::array<vk::DescriptorSetLayoutBinding, 1> uniformBinding;
    uniformBinding[0].binding = 0;
    uniformBinding[0].descriptorCount = 1;
    uniformBinding[0].descriptorType = vk::DescriptorType::eUniformBuffer;
    uniformBinding[0].stageFlags = vk::ShaderStageFlagBits::eVertex;


    uniformBinding[1].binding = 1;
    uniformBinding[1].descriptorCount = 1;
    uniformBinding[1].descriptorType = vk::DescriptorType::eUniformBuffer;
    uniformBinding[1].stageFlags = vk::ShaderStageFlagBits::eVertex;
    info.bindingCount = 2;
    info.pBindings = uniformBinding.data();
    mDescSetLayouts[DESCRIPTOR_TYPE_UNIFORM] = gRuntimeGlobalContext.getRHI()->mDevice.createDescriptorSetLayout(info);

    // 变动较多的 sample DescriptorSetLayout
    vk::DescriptorSetLayoutBinding sampleBinding;
    sampleBinding.binding = 0;
    sampleBinding.descriptorCount = 1;
    sampleBinding.descriptorType = vk::DescriptorType::eCombinedImageSampler;
    sampleBinding.stageFlags = vk::ShaderStageFlagBits::eFragment;
    info.bindingCount = 1;
    info.pBindings = &sampleBinding;
    mDescSetLayouts[DESCRIPTOR_TYPE_SAMPLE] = gRuntimeGlobalContext.getRHI()->mDevice.createDescriptorSetLayout(info);
}
