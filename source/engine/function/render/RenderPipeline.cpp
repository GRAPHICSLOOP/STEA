#include "RenderPipeline.h"
#include "function/global/RuntimeGlobalContext.h"
#include "VulkanUtil.h"
#include "core/base/macro.h"

RenderPipeline::~RenderPipeline()
{
    for (auto& iter : mFrame.mAttachments)
    {
        gRuntimeGlobalContext.getRHI()->mDevice.freeMemory(iter.second.mMemory);
        gRuntimeGlobalContext.getRHI()->mDevice.destroyImageView(iter.second.mImageView);
        gRuntimeGlobalContext.getRHI()->mDevice.destroyImage(iter.second.mImage);
    }
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
        std::vector<vk::DescriptorSetLayout> descSetLayouts(2);
        descSetLayouts[0] = gRuntimeGlobalContext.getRenderResource()->mUniformResource->mDescSetLayout;
        descSetLayouts[1] = gRuntimeGlobalContext.getRenderResource()->getDescriptorSetLayout(DESCRIPTOR_TYPE::DT_Sample);


        // vertex Descriptions
        vk::PipelineVertexInputStateCreateInfo vertexInfo;
        auto VertexAttributeDescriptions = VertexResource::getInputAttributes({ VertexAttribute::VA_Position,VertexAttribute::VA_Color,VertexAttribute::VA_UV0 });
        auto VertexBindingDescriptions = VertexResource::getBindingDescription({ VertexAttribute::VA_Position,VertexAttribute::VA_Color,VertexAttribute::VA_UV0 });
        vertexInfo.vertexAttributeDescriptionCount = VertexAttributeDescriptions.size();
        vertexInfo.vertexBindingDescriptionCount = VertexBindingDescriptions.size();
        vertexInfo.pVertexAttributeDescriptions = VertexAttributeDescriptions.data();
        vertexInfo.pVertexBindingDescriptions = VertexBindingDescriptions.data();

        // shaderStatus
        std::vector<vk::PipelineShaderStageCreateInfo> shaderStatus(2);
        vk::PipelineShaderStageCreateInfo& vertexShader = shaderStatus[0];
        vk::ShaderModule vertShaderModule = VulkanUtil::loadShaderModuleFromFile("shaders/vert.spv");
        vk::ShaderModule fragShaderModule = VulkanUtil::loadShaderModuleFromFile("shaders/frag.spv");
        vertexShader.module = vertShaderModule;
        vertexShader.pName = "main";
        vertexShader.stage = vk::ShaderStageFlagBits::eVertex;
        vk::PipelineShaderStageCreateInfo& fragShader = shaderStatus[1];
        fragShader.module = fragShaderModule;
        fragShader.pName = "main";
        fragShader.stage = vk::ShaderStageFlagBits::eFragment;

        // pushConstants
        vk::PushConstantRange pushRange;
        pushRange.offset = 0;
        pushRange.size = sizeof(ObjectBufferData);
        pushRange.stageFlags = vk::ShaderStageFlagBits::eVertex;

        // pipeline layout
        vk::PipelineLayoutCreateInfo pipelineLayoutInfo;
        pipelineLayoutInfo.setLayoutCount = (uint32_t)descSetLayouts.size();
        pipelineLayoutInfo.pSetLayouts = descSetLayouts.data();
        pipelineLayoutInfo.pPushConstantRanges = &pushRange;
        pipelineLayoutInfo.pushConstantRangeCount = 1;
        vk::PipelineLayout pipelineLayout = gRuntimeGlobalContext.getRHI()->mDevice.createPipelineLayout(pipelineLayoutInfo);

        mCameraPass = std::make_shared<MainCameraPass>();
        mCameraPass->mColorBlendAttachmentCount = 1;
        mCameraPass->initialize(vertexInfo, shaderStatus, pipelineLayout, mFrame.mRenderPass);

        gRuntimeGlobalContext.getRHI()->mDevice.destroyShaderModule(vertShaderModule);
        gRuntimeGlobalContext.getRHI()->mDevice.destroyShaderModule(fragShaderModule);
    }
	

	mUIPass = std::make_shared<UIPass>();
	mUIPass->initialize(UIPassConfigParam(mFrame.mRenderPass));
}

void RenderPipeline::draw()
{
    beginDraw();

	mCameraPass->drawPass();
	mUIPass->drawPass();
    
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
    passBegineInfo.clearValueCount = clearValues.size();


    gRuntimeGlobalContext.getRHI()->mCommandBuffer.beginRenderPass(passBegineInfo, vk::SubpassContents::eInline);

}

void RenderPipeline::endDraw()
{
    gRuntimeGlobalContext.getRHI()->mCommandBuffer.endRenderPass();
}

void RenderPipeline::createAttachment()
{
    FrameBufferAttachment depthFrameBufferAttachment;
    depthFrameBufferAttachment.mFormat = vk::Format::eD24UnormS8Uint;
    VulkanUtil::createImage(
        gRuntimeGlobalContext.getRHI()->mSwapchainSupportDetails.mExtent2D.width,
        gRuntimeGlobalContext.getRHI()->mSwapchainSupportDetails.mExtent2D.height,
        depthFrameBufferAttachment.mFormat,
        vk::ImageTiling::eOptimal,
        vk::ImageUsageFlagBits::eDepthStencilAttachment | vk::ImageUsageFlagBits::eInputAttachment,
        vk::MemoryPropertyFlagBits::eDeviceLocal,
        depthFrameBufferAttachment.mImage,
        depthFrameBufferAttachment.mMemory);
    depthFrameBufferAttachment.mImageView = VulkanUtil::createImageView(
        vk::ImageAspectFlagBits::eDepth,
        depthFrameBufferAttachment.mFormat,
        depthFrameBufferAttachment.mImage);

    FrameBufferAttachment colorFrameBufferAttachment;
    colorFrameBufferAttachment.mFormat = gRuntimeGlobalContext.getRHI()->mSwapchainSupportDetails.mFormat.format;
    VulkanUtil::createImage(
        gRuntimeGlobalContext.getRHI()->mSwapchainSupportDetails.mExtent2D.width,
        gRuntimeGlobalContext.getRHI()->mSwapchainSupportDetails.mExtent2D.height,
        colorFrameBufferAttachment.mFormat,
        vk::ImageTiling::eOptimal,
        vk::ImageUsageFlagBits::eColorAttachment | vk::ImageUsageFlagBits::eInputAttachment,
        vk::MemoryPropertyFlagBits::eDeviceLocal,
        colorFrameBufferAttachment.mImage,
        colorFrameBufferAttachment.mMemory
    );
    colorFrameBufferAttachment.mImageView = VulkanUtil::createImageView(
        vk::ImageAspectFlagBits::eColor,
        colorFrameBufferAttachment.mFormat,
        colorFrameBufferAttachment.mImage);

    FrameBufferAttachment normalFrameBufferAttachment;
    normalFrameBufferAttachment.mFormat = vk::Format::eR8G8B8A8Unorm;
    VulkanUtil::createImage(
        gRuntimeGlobalContext.getRHI()->mSwapchainSupportDetails.mExtent2D.width,
        gRuntimeGlobalContext.getRHI()->mSwapchainSupportDetails.mExtent2D.height,
        normalFrameBufferAttachment.mFormat,
        vk::ImageTiling::eOptimal,
        vk::ImageUsageFlagBits::eColorAttachment | vk::ImageUsageFlagBits::eInputAttachment,
        vk::MemoryPropertyFlagBits::eDeviceLocal,
        normalFrameBufferAttachment.mImage,
        normalFrameBufferAttachment.mMemory
    );
    normalFrameBufferAttachment.mImageView = VulkanUtil::createImageView(
        vk::ImageAspectFlagBits::eColor,
        normalFrameBufferAttachment.mFormat ,
        normalFrameBufferAttachment.mImage);


    mFrame.mAttachments[ATTACHMENT_TYPE::Depth] = depthFrameBufferAttachment;
    mFrame.mAttachments[ATTACHMENT_TYPE::Color] = colorFrameBufferAttachment;
    mFrame.mAttachments[ATTACHMENT_TYPE::Normal] = normalFrameBufferAttachment;
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
    colorAttachmentDesc.format = mFrame.mAttachments[ATTACHMENT_TYPE::Color].mFormat;
    colorAttachmentDesc.samples = vk::SampleCountFlagBits::e1;
    colorAttachmentDesc.loadOp = vk::AttachmentLoadOp::eClear;
    colorAttachmentDesc.storeOp = vk::AttachmentStoreOp::eDontCare;
    colorAttachmentDesc.initialLayout = vk::ImageLayout::eUndefined;
    colorAttachmentDesc.finalLayout = vk::ImageLayout::eColorAttachmentOptimal;
    colorAttachmentDesc.stencilLoadOp = vk::AttachmentLoadOp::eDontCare;
    colorAttachmentDesc.stencilStoreOp = vk::AttachmentStoreOp::eDontCare;

    // normal attachment
    vk::AttachmentDescription& normalAttachmentDesc = attachmentDesces[2];
    normalAttachmentDesc.format = mFrame.mAttachments[ATTACHMENT_TYPE::Normal].mFormat;
    normalAttachmentDesc.samples = vk::SampleCountFlagBits::e1;
    normalAttachmentDesc.loadOp = vk::AttachmentLoadOp::eClear;
    normalAttachmentDesc.storeOp = vk::AttachmentStoreOp::eDontCare;
    normalAttachmentDesc.initialLayout = vk::ImageLayout::eUndefined;
    normalAttachmentDesc.finalLayout = vk::ImageLayout::eColorAttachmentOptimal;
    normalAttachmentDesc.stencilLoadOp = vk::AttachmentLoadOp::eDontCare;
    normalAttachmentDesc.stencilStoreOp = vk::AttachmentStoreOp::eDontCare;

    // depth stencil attachment
    vk::AttachmentDescription& depthAttachmentDesc = attachmentDesces[3];
    depthAttachmentDesc.format = mFrame.mAttachments[ATTACHMENT_TYPE::Depth].mFormat;
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

    //std::vector<vk::SubpassDescription> subpassDesc(2);
    //subpassDesc[0].pipelineBindPoint = vk::PipelineBindPoint::eGraphics;
    //subpassDesc[0].colorAttachmentCount = 2;
    //subpassDesc[0].pColorAttachments = colorAttachmentRef;
    //subpassDesc[0].pDepthStencilAttachment = depthAttachmentRef;

    //subpassDesc[1].pipelineBindPoint = vk::PipelineBindPoint::eGraphics;
    //subpassDesc[1].colorAttachmentCount = 1;
    //subpassDesc[1].pColorAttachments = swapAttachmentRef;
    //subpassDesc[1].inputAttachmentCount = 3;
    //subpassDesc[1].pInputAttachments = inputAttachmentRef;

    //std::vector<vk::SubpassDependency> subpassDependency(2);
    //subpassDependency[0].srcSubpass = VK_SUBPASS_EXTERNAL; // 因为我们不依赖任何subpass
    //subpassDependency[0].dstSubpass = 0;
    //subpassDependency[0].srcStageMask = vk::PipelineStageFlagBits::eTopOfPipe;
    //subpassDependency[0].dstStageMask = vk::PipelineStageFlagBits::eColorAttachmentOutput;
    //subpassDependency[0].srcAccessMask = vk::AccessFlagBits::eNone;
    //subpassDependency[0].dstAccessMask = vk::AccessFlagBits::eColorAttachmentWrite;

    //subpassDependency[1].srcSubpass = 0; // 因为我们不依赖任何subpass
    //subpassDependency[1].dstSubpass = 1;
    //subpassDependency[1].srcStageMask = vk::PipelineStageFlagBits::eColorAttachmentOutput;
    //subpassDependency[1].dstStageMask = vk::PipelineStageFlagBits::eFragmentShader;
    //subpassDependency[1].srcAccessMask = vk::AccessFlagBits::eColorAttachmentWrite;
    //subpassDependency[1].dstAccessMask = vk::AccessFlagBits::eShaderRead;

    vk::AttachmentReference attachmentRef[2];
    attachmentRef[0].attachment = 0;
    attachmentRef[0].layout = vk::ImageLayout::eColorAttachmentOptimal;
    attachmentRef[1].attachment = 3;
    attachmentRef[1].layout = vk::ImageLayout::eDepthStencilAttachmentOptimal;

    vk::SubpassDescription subpassDesc;
    subpassDesc.colorAttachmentCount = 1;
    subpassDesc.pColorAttachments = &attachmentRef[0];
    subpassDesc.pDepthStencilAttachment = &attachmentRef[1];
    subpassDesc.pipelineBindPoint = vk::PipelineBindPoint::eGraphics;

    vk::SubpassDependency subpassDependency;
    subpassDependency.srcSubpass = VK_SUBPASS_EXTERNAL; // 因为我们不依赖任何subpass
    subpassDependency.dstSubpass = 0;
    subpassDependency.srcStageMask = vk::PipelineStageFlagBits::eTopOfPipe;
    subpassDependency.dstStageMask = vk::PipelineStageFlagBits::eColorAttachmentOutput;
    subpassDependency.srcAccessMask = vk::AccessFlagBits::eNone;
    subpassDependency.dstAccessMask = vk::AccessFlagBits::eColorAttachmentWrite;

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
        attachments[1] = mFrame.mAttachments[ATTACHMENT_TYPE::Color].mImageView;
        attachments[2] = mFrame.mAttachments[ATTACHMENT_TYPE::Normal].mImageView;
        attachments[3] = mFrame.mAttachments[ATTACHMENT_TYPE::Depth].mImageView;

        mFrame.mFramebuffer[i] = gRuntimeGlobalContext.getRHI()->mDevice.createFramebuffer(info);
        CHECK_NULL(mFrame.mFramebuffer[i]);
    }
}
