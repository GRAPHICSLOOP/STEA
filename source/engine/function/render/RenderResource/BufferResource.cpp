#include "BufferResource.h"
#include "../VulkanUtil.h"
#include "../../global/RuntimeGlobalContext.h"
#include "RenderData.h"

BufferResource::~BufferResource()
{
    gRuntimeGlobalContext.getRHI()->mDevice.destroyDescriptorSetLayout(mDescSetLayout);
    for (const auto& atrr : mBufferAttributes)
    {
        vkUnmapMemory(gRuntimeGlobalContext.getRHI()->mDevice, atrr.mBufferMemory);
        gRuntimeGlobalContext.getRHI()->mDevice.destroyBuffer(atrr.mBuffer);
        gRuntimeGlobalContext.getRHI()->mDevice.freeMemory(atrr.mBufferMemory);
    }

}

std::shared_ptr<BufferResource> BufferResource::create(const std::vector<BufferAttributes>& bufferAttributes)
{
    struct make_shared_enabler : public BufferResource {};
    std::shared_ptr<BufferResource> bufferResource = std::make_shared<make_shared_enabler>();

    bufferResource->mBufferAttributes = bufferAttributes;
    bufferResource->setUpAlignment();

    for (auto& atrr : bufferResource->mBufferAttributes)
    {
        VulkanUtil::createBuffer(atrr.mBufferSize, vk::BufferUsageFlagBits::eUniformBuffer,
            vk::MemoryPropertyFlagBits::eHostCoherent | vk::MemoryPropertyFlagBits::eHostVisible, atrr.mBuffer,
            atrr.mBufferMemory);

        vkMapMemory(gRuntimeGlobalContext.getRHI()->mDevice, atrr.mBufferMemory, 0, atrr.mBufferSize, 0, &atrr.mData);
    }

    bufferResource->createDescriptorLayout();
    bufferResource->createDescriptorSet();

    return bufferResource;
}

void* BufferResource::getData(UNIFORMBUFFERTYPE type)
{
    for (auto& aligment : mBufferAttributes)
    {
        if(aligment.mType == type)
            return aligment.mData;
    }
    
    return nullptr;
}

void BufferResource::getDynamicOffsets(std::vector<uint32_t>& offset, const uint32_t index)
{
    std::vector<uint32_t> result;
    for (uint32_t i = 0; i < mBufferAttributes.size(); i++)
    {
        if (mBufferAttributes[i].mDescriptorType == vk::DescriptorType::eUniformBufferDynamic)
        {
            result.push_back(mBufferAttributes[i].mAlignments * index);
        }
    }

    offset = result;
}

void BufferResource::setUpAlignment()
{    
    // 数据对齐
    VkPhysicalDeviceProperties properties;
    vkGetPhysicalDeviceProperties(gRuntimeGlobalContext.getRHI()->mPhyDevice, &properties);
    uint32_t minUboAlignment = (uint32_t)properties.limits.minUniformBufferOffsetAlignment;

    for (uint32_t i = 0; i < mBufferAttributes.size(); i++)
    {
        uint32_t alignmentSize = (mBufferAttributes[i].mSize + minUboAlignment - 1) & ~(minUboAlignment - 1); // gpu上的对齐规则
        mBufferAttributes[i].mAlignments = alignmentSize;
        if (i - 1 <= 0)
            mBufferAttributes[i].mOffset = mBufferAttributes[i - 1].mAlignments;

        alignmentSize *= mBufferAttributes[i].mCount;
        mBufferAttributes[i].mBufferSize = alignmentSize;
    }

}

void BufferResource::createDescriptorLayout()
{
    // DynamicUniform DescriptorSetLayout
    vk::DescriptorSetLayoutCreateInfo info;
    std::vector< vk::DescriptorSetLayoutBinding> binding;
    binding.resize(mBufferAttributes.size());

    for (uint32_t i = 0; i < mBufferAttributes.size(); i++)
    {
        binding[i].binding = i;
        binding[i].descriptorCount = 1;
        binding[i].descriptorType = mBufferAttributes[i].mDescriptorType;
        binding[i].stageFlags = mBufferAttributes[i].mShaderStage;
    }

    info.bindingCount = (uint32_t)binding.size();
    info.pBindings = binding.data();
    mDescSetLayout = gRuntimeGlobalContext.getRHI()->mDevice.createDescriptorSetLayout(info);
}

void BufferResource::createDescriptorSet()
{
    vk::DescriptorSetAllocateInfo info;
    info.descriptorPool = gRuntimeGlobalContext.getRHI()->mDescriptorPool;
    info.descriptorSetCount = 1;
    info.pSetLayouts = &mDescSetLayout;
    mDescriptorSet = gRuntimeGlobalContext.getRHI()->mDevice.allocateDescriptorSets(info)[0];


    for (uint32_t i = 0; i < mBufferAttributes.size(); i++)
    {
        vk::WriteDescriptorSet writeSet;
        vk::DescriptorBufferInfo info;
        info.buffer = mBufferAttributes[i].mBuffer;
        info.offset = 0;
        info.range = mBufferAttributes[i].mSize;


        // 更新描述符
        writeSet.dstArrayElement = 0;
        writeSet.dstBinding = i;
        writeSet.dstSet = mDescriptorSet;
        writeSet.descriptorType = mBufferAttributes[i].mDescriptorType;
        writeSet.descriptorCount = 1;
        writeSet.pBufferInfo = &info;

        gRuntimeGlobalContext.getRHI()->mDevice.updateDescriptorSets(writeSet, nullptr);
    }

}

