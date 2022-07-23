#include "RenderPipeline.h"
#include "function/global/RuntimeGlobalContext.h"
#include "VulkanUtil.h"
#include "core/base/macro.h"
#include "shader/Shader.h"

RenderPipeline::~RenderPipeline()
{
    gRuntimeGlobalContext.getRHI()->mDevice.destroyRenderPass(mFrame.mRenderPass);
    for (auto& framebuffer : mFrame.mFramebuffer)
    {
        gRuntimeGlobalContext.getRHI()->mDevice.destroyFramebuffer(framebuffer);
    }

}

void RenderPipeline::initialize()
{
    createAttachment();
    createRenderPass();
    createFrameBuffer();

    // maincamerapass
    {
        // vertex Descriptions
        vk::PipelineVertexInputStateCreateInfo vertexInfo;
        auto VertexAttributeDescriptions = VertexResource::getInputAttributes({ VertexAttribute::VA_Position,VertexAttribute::VA_Color,VertexAttribute::VA_UV0 });
        auto VertexBindingDescriptions = VertexResource::getBindingDescription({ VertexAttribute::VA_Position,VertexAttribute::VA_Color,VertexAttribute::VA_UV0 });
        vertexInfo.vertexAttributeDescriptionCount = (uint32_t)VertexAttributeDescriptions.size();
        vertexInfo.vertexBindingDescriptionCount = (uint32_t)VertexBindingDescriptions.size();
        vertexInfo.pVertexAttributeDescriptions = VertexAttributeDescriptions.data();
        vertexInfo.pVertexBindingDescriptions = VertexBindingDescriptions.data();

        // pushConstants
        vk::PushConstantRange pushRange;
        pushRange.offset = 0;
        pushRange.size = sizeof(ObjectBufferData);
        pushRange.stageFlags = vk::ShaderStageFlagBits::eVertex;

        mCameraPass = std::make_shared<MainCameraPass>();
        mCameraPass->mColorBlendAttachmentCount = 2;
        mCameraPass->mPushRange.push_back(pushRange);
        mCameraPass->initialize(vertexInfo, gRuntimeGlobalContext.getRenderResource()->getShader("obj"),  mFrame.mRenderPass);
    }

    // postprocesspass
    {
        // vertex Descriptions
        vk::PipelineVertexInputStateCreateInfo vertexInfo;
        auto VertexAttributeDescriptions = VertexResource::getInputAttributes({ VertexAttribute::VA_Position,VertexAttribute::VA_UV0 });
        auto VertexBindingDescriptions = VertexResource::getBindingDescription({ VertexAttribute::VA_Position,VertexAttribute::VA_UV0 });
        vertexInfo.vertexAttributeDescriptionCount = (uint32_t)VertexAttributeDescriptions.size();
        vertexInfo.vertexBindingDescriptionCount = (uint32_t)VertexBindingDescriptions.size();
        vertexInfo.pVertexAttributeDescriptions = VertexAttributeDescriptions.data();
        vertexInfo.pVertexBindingDescriptions = VertexBindingDescriptions.data();

        mPostProcessPass = std::make_shared<PostProcessPass>();
        mPostProcessPass->mSubpassIndex = 1;
        mPostProcessPass->mDepthInfo.depthTestEnable = VK_FALSE;
        mPostProcessPass->mDepthInfo.depthWriteEnable = VK_FALSE;
        mPostProcessPass->mDepthInfo.stencilTestEnable = VK_FALSE;
        mPostProcessPass->initialize(vertexInfo, gRuntimeGlobalContext.getRenderResource()->getShader("quad"), mFrame.mRenderPass);
        //mPostProcessPass->createDescriptorSet(mFrame);
    }
	
    // 待优化项
    // 
    /*
    * DescriptorSetLayout 是否可以优化一下框架，目前是分散销毁，创建。
    * 可能得通过SPIRV编译器在Editor中对Shader进行编译，然后获得相关的信息，因此得统一管理shader
    */
    // 然后得产出两篇文章，一篇是SPIRV编译器自动化生成各种信息的，一篇是subpass的理解笔记

	//mUIPass = std::make_shared<UIPass>();
	//mUIPass->initialize(UIPassConfigParam(mFrame.mRenderPass));
}

void RenderPipeline::draw()
{
    beginDraw();
    vk::CommandBuffer cmdBuffer = gRuntimeGlobalContext.getRHI()->mCommandBuffer;

	mCameraPass->drawPass(cmdBuffer);
    cmdBuffer.nextSubpass(vk::SubpassContents::eInline);
    mPostProcessPass->drawPass(cmdBuffer);
	//mUIPass->drawPass();
    
    endDraw();
}

void RenderPipeline::beginDraw()
{
    vk::RenderPassBeginInfo passBegineInfo;
    passBegineInfo.renderPass = mFrame.mRenderPass;
    passBegineInfo.framebuffer = mFrame.mFramebuffer[gRuntimeGlobalContext.getRHI()->getNextImageIndex()];
    vk::Rect2D area;
    area.offset = 0;
    area.extent = gRuntimeGlobalContext.getRHI()->mSwapchainSupportDetails.mExtent2D;
    passBegineInfo.setRenderArea(area);

    std::array<vk::ClearValue, 4> clearValues;
    clearValues[0].color = std::array<float, 4>{1, 1, 1, 1.f};
    clearValues[1].color = std::array<float, 4>{0, 0, 0, 0};
    clearValues[2].color = std::array<float, 4>{0, 0, 0, 0};
    clearValues[3].depthStencil = 1.f;
    passBegineInfo.pClearValues = clearValues.data();
    passBegineInfo.clearValueCount = (uint32_t)clearValues.size();


    gRuntimeGlobalContext.getRHI()->mCommandBuffer.beginRenderPass(passBegineInfo, vk::SubpassContents::eInline);

}

void RenderPipeline::endDraw()
{
    gRuntimeGlobalContext.getRHI()->mCommandBuffer.endRenderPass();
}

void RenderPipeline::createAttachment()
{
    std::shared_ptr<ImageResource> depthFrameBufferAttachment = ImageResource::createAttachment(
        gRuntimeGlobalContext.getRHI()->mSwapchainSupportDetails.mExtent2D.width,
        gRuntimeGlobalContext.getRHI()->mSwapchainSupportDetails.mExtent2D.height,
        vk::ImageUsageFlagBits::eDepthStencilAttachment | vk::ImageUsageFlagBits::eInputAttachment,
        vk::ImageAspectFlagBits::eDepth,
        vk::Format::eD24UnormS8Uint,
        gRuntimeGlobalContext.getRenderResource()->getShader("quad"),
        "inputDepth"
    );

    std::shared_ptr<ImageResource> colorFrameBufferAttachment = ImageResource::createAttachment(
        gRuntimeGlobalContext.getRHI()->mSwapchainSupportDetails.mExtent2D.width,
        gRuntimeGlobalContext.getRHI()->mSwapchainSupportDetails.mExtent2D.height,
        vk::ImageUsageFlagBits::eColorAttachment | vk::ImageUsageFlagBits::eInputAttachment,
        vk::ImageAspectFlagBits::eColor,
        gRuntimeGlobalContext.getRHI()->mSwapchainSupportDetails.mFormat.format,
        gRuntimeGlobalContext.getRenderResource()->getShader("quad"),
        "inputColor"
    );

    std::shared_ptr<ImageResource> normalFrameBufferAttachment = ImageResource::createAttachment(
        gRuntimeGlobalContext.getRHI()->mSwapchainSupportDetails.mExtent2D.width,
        gRuntimeGlobalContext.getRHI()->mSwapchainSupportDetails.mExtent2D.height,
        vk::ImageUsageFlagBits::eColorAttachment | vk::ImageUsageFlagBits::eInputAttachment,
        vk::ImageAspectFlagBits::eColor,
        vk::Format::eR8G8B8A8Unorm,
        gRuntimeGlobalContext.getRenderResource()->getShader("quad"),
        "inputNormal"
    );

    mFrame.mAttachments.resize(3);
    mFrame.mAttachments[0] = depthFrameBufferAttachment;
    mFrame.mAttachments[1] = colorFrameBufferAttachment;
    mFrame.mAttachments[2] = normalFrameBufferAttachment;
}

void RenderPipeline::createRenderPass()
{
    std::array<vk::AttachmentDescription, 4> attachmentDesces;

    // swap chain attachment
    vk::AttachmentDescription& swapAttachmentDesc = attachmentDesces[0];
    swapAttachmentDesc.format = gRuntimeGlobalContext.getRHI()->mSwapchainSupportDetails.mFormat.format;
    swapAttachmentDesc.samples = vk::SampleCountFlagBits::e1;
    swapAttachmentDesc.loadOp = vk::AttachmentLoadOp::eClear;
    swapAttachmentDesc.storeOp = vk::AttachmentStoreOp::eStore;
    swapAttachmentDesc.initialLayout = vk::ImageLayout::eUndefined;
    swapAttachmentDesc.finalLayout = vk::ImageLayout::ePresentSrcKHR;
    swapAttachmentDesc.stencilLoadOp = vk::AttachmentLoadOp::eDontCare;
    swapAttachmentDesc.stencilStoreOp = vk::AttachmentStoreOp::eDontCare;

    // color attachment
    vk::AttachmentDescription& colorAttachmentDesc = attachmentDesces[1];
    colorAttachmentDesc.format = mFrame.mAttachments[1]->mImageBufferResource.mFormat;
    colorAttachmentDesc.samples = vk::SampleCountFlagBits::e1;
    colorAttachmentDesc.loadOp = vk::AttachmentLoadOp::eClear;
    colorAttachmentDesc.storeOp = vk::AttachmentStoreOp::eDontCare;
    colorAttachmentDesc.initialLayout = vk::ImageLayout::eUndefined;
    colorAttachmentDesc.finalLayout = vk::ImageLayout::eColorAttachmentOptimal;
    colorAttachmentDesc.stencilLoadOp = vk::AttachmentLoadOp::eDontCare;
    colorAttachmentDesc.stencilStoreOp = vk::AttachmentStoreOp::eDontCare;

    // normal attachment
    vk::AttachmentDescription& normalAttachmentDesc = attachmentDesces[2];
    normalAttachmentDesc.format = mFrame.mAttachments[2]->mImageBufferResource.mFormat;
    normalAttachmentDesc.samples = vk::SampleCountFlagBits::e1;
    normalAttachmentDesc.loadOp = vk::AttachmentLoadOp::eClear;
    normalAttachmentDesc.storeOp = vk::AttachmentStoreOp::eDontCare;
    normalAttachmentDesc.initialLayout = vk::ImageLayout::eUndefined;
    normalAttachmentDesc.finalLayout = vk::ImageLayout::eColorAttachmentOptimal;
    normalAttachmentDesc.stencilLoadOp = vk::AttachmentLoadOp::eDontCare;
    normalAttachmentDesc.stencilStoreOp = vk::AttachmentStoreOp::eDontCare;

    // depth stencil attachment
    vk::AttachmentDescription& depthAttachmentDesc = attachmentDesces[3];
    depthAttachmentDesc.format = mFrame.mAttachments[0]->mImageBufferResource.mFormat;
    depthAttachmentDesc.samples = vk::SampleCountFlagBits::e1;
    depthAttachmentDesc.loadOp = vk::AttachmentLoadOp::eClear;
    depthAttachmentDesc.storeOp = vk::AttachmentStoreOp::eDontCare;
    depthAttachmentDesc.initialLayout = vk::ImageLayout::eUndefined;
    depthAttachmentDesc.finalLayout = vk::ImageLayout::eDepthStencilAttachmentOptimal;
    depthAttachmentDesc.stencilLoadOp = vk::AttachmentLoadOp::eDontCare;
    depthAttachmentDesc.stencilStoreOp = vk::AttachmentStoreOp::eDontCare;

    vk::AttachmentReference colorAttachmentRef[2];
    colorAttachmentRef[0].attachment = 1;
    colorAttachmentRef[0].layout = vk::ImageLayout::eColorAttachmentOptimal;
    colorAttachmentRef[1].attachment = 2;
    colorAttachmentRef[1].layout = vk::ImageLayout::eColorAttachmentOptimal;

    vk::AttachmentReference swapAttachmentRef[1];
    swapAttachmentRef[0].attachment = 0;
    swapAttachmentRef[0].layout = vk::ImageLayout::eColorAttachmentOptimal;

    vk::AttachmentReference depthAttachmentRef[1];
    depthAttachmentRef[0].attachment = 3;
    depthAttachmentRef[0].layout = vk::ImageLayout::eDepthStencilAttachmentOptimal;

    vk::AttachmentReference inputAttachmentRef[3];
    inputAttachmentRef[0].attachment = 1;
    inputAttachmentRef[0].layout = vk::ImageLayout::eShaderReadOnlyOptimal;
    inputAttachmentRef[1].attachment = 2;
    inputAttachmentRef[1].layout = vk::ImageLayout::eShaderReadOnlyOptimal;
    inputAttachmentRef[2].attachment = 3;
    inputAttachmentRef[2].layout = vk::ImageLayout::eShaderReadOnlyOptimal;

    std::vector<vk::SubpassDescription> subpassDesc(2);
    subpassDesc[0].pipelineBindPoint = vk::PipelineBindPoint::eGraphics;
    subpassDesc[0].colorAttachmentCount = 2;
    subpassDesc[0].pColorAttachments = colorAttachmentRef;
    subpassDesc[0].pDepthStencilAttachment = depthAttachmentRef;

    subpassDesc[1].pipelineBindPoint = vk::PipelineBindPoint::eGraphics;
    subpassDesc[1].colorAttachmentCount = 1;
    subpassDesc[1].pColorAttachments = swapAttachmentRef;
    subpassDesc[1].inputAttachmentCount = 3;
    subpassDesc[1].pInputAttachments = inputAttachmentRef;

    std::vector<vk::SubpassDependency> subpassDependency(2);
    subpassDependency[0].srcSubpass = VK_SUBPASS_EXTERNAL; // 因为我们不依赖任何subpass
    subpassDependency[0].dstSubpass = 0;
    subpassDependency[0].srcStageMask = vk::PipelineStageFlagBits::eTopOfPipe;
    subpassDependency[0].dstStageMask = vk::PipelineStageFlagBits::eColorAttachmentOutput;
    subpassDependency[0].srcAccessMask = vk::AccessFlagBits::eNone;
    subpassDependency[0].dstAccessMask = vk::AccessFlagBits::eColorAttachmentWrite;

    subpassDependency[1].srcSubpass = 0; // 因为我们不依赖任何subpass
    subpassDependency[1].dstSubpass = 1;
    subpassDependency[1].srcStageMask = vk::PipelineStageFlagBits::eColorAttachmentOutput;
    subpassDependency[1].dstStageMask = vk::PipelineStageFlagBits::eFragmentShader;
    subpassDependency[1].srcAccessMask = vk::AccessFlagBits::eColorAttachmentWrite;
    subpassDependency[1].dstAccessMask = vk::AccessFlagBits::eShaderRead;

    vk::RenderPassCreateInfo info;
    info.setAttachments(attachmentDesces);
    info.setDependencies(subpassDependency);
    info.setSubpasses(subpassDesc);

    mFrame.mRenderPass = gRuntimeGlobalContext.getRHI()->mDevice.createRenderPass(info);
    CHECK_NULL(mFrame.mRenderPass);
}

void RenderPipeline::createFrameBuffer()
{
    mFrame.mFramebuffer.resize(gRuntimeGlobalContext.getRHI()->mSwapchainSupportDetails.mImageCount);
    
    std::array<vk::ImageView, 4> attachments;
    vk::FramebufferCreateInfo info;
    info.renderPass = mFrame.mRenderPass;
    info.pAttachments = attachments.data();
    info.attachmentCount = (uint32_t)attachments.size();
    info.height = gRuntimeGlobalContext.getRHI()->mSwapchainSupportDetails.mExtent2D.height;
    info.width = gRuntimeGlobalContext.getRHI()->mSwapchainSupportDetails.mExtent2D.width;
    info.layers = 1;

    for (uint32_t i = 0; i < gRuntimeGlobalContext.getRHI()->mSwapchainSupportDetails.mImageCount; i++)
    {
        attachments[0] = gRuntimeGlobalContext.getRHI()->mSwapchainImageViews[i];
        attachments[1] = mFrame.mAttachments[1]->mImageBufferResource.mImageInfo.imageView;
        attachments[2] = mFrame.mAttachments[2]->mImageBufferResource.mImageInfo.imageView;
        attachments[3] = mFrame.mAttachments[0]->mImageBufferResource.mImageInfo.imageView;

        mFrame.mFramebuffer[i] = gRuntimeGlobalContext.getRHI()->mDevice.createFramebuffer(info);
        CHECK_NULL(mFrame.mFramebuffer[i]);
    }
}
