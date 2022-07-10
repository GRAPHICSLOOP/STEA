#include "MainCameraPass.h"
#include "core/base/macro.h"
#include "function/global/RuntimeGlobalContext.h"
#include "../VulkanUtil.h"
#include "../RenderResource/VertexResource.h"
#include <glm/gtc/matrix_transform.hpp>

void MainCameraPass::drawPass()
{
    gRuntimeGlobalContext.getRHI()->mCommandBuffer.bindPipeline(vk::PipelineBindPoint::eGraphics, mPipeline);
    vk::DeviceSize offset = 0;

    uint32_t count = 0;
    for (const auto& iter : gRuntimeGlobalContext.getRenderResource()->mModelRenderResources)
    {
        ObjectBufferData model;
        model.mModel = glm::translate(glm::mat4(1.f), glm::vec3((float)count));
        gRuntimeGlobalContext.getRHI()->mCommandBuffer.pushConstants(
            mPipelineLayout, vk::ShaderStageFlagBits::eVertex, 0,sizeof(ObjectBufferData),
            &model);

        std::vector<uint32_t> offsetDynamic;
        gRuntimeGlobalContext.getRenderResource()->mUniformResource->getDynamicOffsets(offsetDynamic, count);

        // 更新模型位置
        gRuntimeGlobalContext.getRHI()->mCommandBuffer.bindDescriptorSets(
            vk::PipelineBindPoint::eGraphics, mPipelineLayout,
            0, 1, &gRuntimeGlobalContext.getRenderResource()->mUniformResource->mDescriptorSet,
            offsetDynamic.size(), offsetDynamic.data());


        // 绑定相关数据和绘制
        for (const auto& resource : iter.second)
        {
            gRuntimeGlobalContext.getRHI()->mCommandBuffer.bindDescriptorSets(vk::PipelineBindPoint::eGraphics,mPipelineLayout,
                1, 1, &resource.mTextureResource.lock()->mTextureBufferResource.mDescriptorSet,
                0, nullptr);
            gRuntimeGlobalContext.getRHI()->mCommandBuffer.bindVertexBuffers(0, 1, &resource.mMeshResource.lock()->mMeshBufferResource.mVertexResource->mBuffer, &offset);
            gRuntimeGlobalContext.getRHI()->mCommandBuffer.bindIndexBuffer(resource.mMeshResource.lock()->mMeshBufferResource.mIndexResource->mBuffer, offset, vk::IndexType::eUint32);
            gRuntimeGlobalContext.getRHI()->mCommandBuffer.drawIndexed(resource.mMeshResource.lock()->mMeshBufferResource.mIndexResource->mIndexCount, 1, 0, 0, 0);
        }

        count++;
    }
    //gRuntimeGlobalContext.getRHI()->mCommandBuffer.endRenderPass();
}

