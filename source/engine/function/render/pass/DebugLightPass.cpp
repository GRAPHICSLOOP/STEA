#include "DebugLightPass.h"
#include "../../global/RuntimeGlobalContext.h"

vk::Pipeline DebugLightPass::initialize(const vk::PipelineVertexInputStateCreateInfo& vertexInfo, const Shader* shader, const vk::RenderPass renderPass)
{
    RenderPassBase::initialize(vertexInfo, shader, renderPass);

	{
        std::vector<float> vertices =
        {
            -0.5f, -0.5f, -0.5f,
            0.5f, -0.5f, -0.5f,
            0.5f, -0.5f, 0.5f,
            -0.5f, -0.5f, 0.5f,

            -0.5f, 0.5f, -0.5f,
            0.5f, 0.5f, -0.5f,
            0.5f, 0.5f, 0.5f,
            -0.5f, 0.5f, 0.5f
        };

        for (auto& v : vertices)
        {
            v *= 0.05f;
        }

        std::vector<uint32_t> indices =
        {
            2,1,0,
            3,2,0,
            4,5,6,
            4,6,7,
            7,6,2,
            7,2,3,
            1,5,4,
            0,1,4,
            6,5,1,
            6,1,2,
            4,7,0,
            0,7,3
        };

		mBoxVertexResource = VertexResource::create(vertices.data(), (uint32_t)vertices.size(), { VertexAttribute::VA_Position });
        mBoxIndexResource = IndexResource::create(indices.data(), (uint32_t)indices.size(), sizeof(uint32_t));
    }

    return mPipeline;
}

void DebugLightPass::drawPass(vk::CommandBuffer cmdBuffer)
{
    cmdBuffer.bindPipeline(vk::PipelineBindPoint::eGraphics, mPipeline);
    vk::DeviceSize offset = 0;

    // 更新模型位置
    cmdBuffer.bindDescriptorSets(
        vk::PipelineBindPoint::eGraphics, mPipelineLayout,
        0, 1, &gRuntimeGlobalContext.getRenderResource()->mLightUniformResource->mDescriptorSet,0,0);

    cmdBuffer.bindVertexBuffers(0, 1, &mBoxVertexResource->mBuffer, &offset);
    cmdBuffer.bindIndexBuffer(mBoxIndexResource->mBuffer, offset, vk::IndexType::eUint32);
    cmdBuffer.drawIndexed(mBoxIndexResource->mIndexCount, LIGHT_MAXNUMB, 0, 0, 0);
}

void DebugLightPass::setDescriptorSet(vk::DescriptorSet descriptorSet)
{
    mDescriptorSet = descriptorSet;
}
