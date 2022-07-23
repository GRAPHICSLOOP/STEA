#include "MainCameraPass.h"
#include "core/base/macro.h"
#include "function/global/RuntimeGlobalContext.h"
#include "../VulkanUtil.h"
#include "../RenderResource/VertexResource.h"
#include <glm/gtc/matrix_transform.hpp>

void MainCameraPass::drawPass(vk::CommandBuffer cmdBuffer)
{
    cmdBuffer.bindPipeline(vk::PipelineBindPoint::eGraphics, mPipeline);
    vk::DeviceSize offset = 0;

    uint32_t count = 0;
    for (const auto& iter : gRuntimeGlobalContext.getRenderResource()->mModelRenderResources)
    {
        std::vector<uint32_t> offsetDynamic;
        gRuntimeGlobalContext.getRenderResource()->mUniformResource->getDynamicOffsets(offsetDynamic, count);

        // 更新模型位置
        cmdBuffer.bindDescriptorSets(
            vk::PipelineBindPoint::eGraphics, mPipelineLayout,
            0, 1, &mShader->mDescriptorSets[0],
            (uint32_t)offsetDynamic.size(), offsetDynamic.data());


        // 绑定相关数据和绘制
        for (const auto& resource : iter.second)
        {
            cmdBuffer.bindDescriptorSets(vk::PipelineBindPoint::eGraphics,mPipelineLayout,
                1, 1, &mShader->mDescriptorSets[1],
                0, nullptr);
            cmdBuffer.bindVertexBuffers(0, 1, &resource.mMeshResource.lock()->mMeshBufferResource.mVertexResource->mBuffer, &offset);
            cmdBuffer.bindIndexBuffer(resource.mMeshResource.lock()->mMeshBufferResource.mIndexResource->mBuffer, offset, vk::IndexType::eUint32);
            cmdBuffer.drawIndexed(resource.mMeshResource.lock()->mMeshBufferResource.mIndexResource->mIndexCount, 1, 0, 0, 0);
        }

        count++;
    }
    //cmdBuffer.endRenderPass();
}

