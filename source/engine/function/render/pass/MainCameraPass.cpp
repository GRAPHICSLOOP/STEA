#include "MainCameraPass.h"
#include "core/base/macro.h"
#include "function/global/RuntimeGlobalContext.h"
#include "../VulkanUtil.h"
#include "../RenderResource/VertexResource.h"

MainCameraPass::~MainCameraPass()
{
    for (uint32_t i = 0; i < mFrame.mFramebuffer.size(); i++)
    {
        gRuntimeGlobalContext.getRHI()->mDevice.destroyFramebuffer(mFrame.mFramebuffer[i]);
    }
    gRuntimeGlobalContext.getRHI()->mDevice.destroyPipelineLayout(mPipelineLayout);
    gRuntimeGlobalContext.getRHI()->mDevice.destroyPipeline(mPipeline);
    gRuntimeGlobalContext.getRHI()->mDevice.destroyRenderPass(mFrame.mRenderPass);

    std::vector<FrameBufferAttachment> attachments = mFrame.getAttachments();
    for (uint32_t i = 0; i < attachments.size(); i++)
    {
        gRuntimeGlobalContext.getRHI()->mDevice.destroyImage(attachments[i].mImage);
        gRuntimeGlobalContext.getRHI()->mDevice.destroyImageView(attachments[i].mImageView);
        gRuntimeGlobalContext.getRHI()->mDevice.freeMemory(attachments[i].mMemory);
    }
}

void MainCameraPass::initialize()
{
    setupAttachments();
    setupRenderPass();
    setupDescriptorSetLayout();
    setupPipelines();
    setupDescriptorSet();
    //setupFramebufferDescriptorSet();
    setupSwapchainFramebuffers();

}

void MainCameraPass::drawPass()
{
    vk::RenderPassBeginInfo passBegineInfo;
    passBegineInfo.renderPass = mFrame.mRenderPass;
    passBegineInfo.framebuffer = mFrame.mFramebuffer[gRuntimeGlobalContext.getRHI()->getNextImageIndex()];
    vk::Rect2D area;
    area.offset = 0;
    area.extent = gRuntimeGlobalContext.getRHI()->mSwapchainSupportDetails.mExtent2D;
    passBegineInfo.setRenderArea(area);

    std::array<vk::ClearValue, 2> clearValues;
    clearValues[0].color = std::array<float, 4>{0, 0, 0, 1.f};
    clearValues[1].depthStencil = 1.f;
    passBegineInfo.pClearValues = clearValues.data();
    passBegineInfo.clearValueCount = 2;

    gRuntimeGlobalContext.getRHI()->mCommandBuffer.beginRenderPass(passBegineInfo, vk::SubpassContents::eInline);
    gRuntimeGlobalContext.getRHI()->mCommandBuffer.bindPipeline(vk::PipelineBindPoint::eGraphics, mPipeline);
    vk::DeviceSize offset = 0;

    gRuntimeGlobalContext.getRHI()->mCommandBuffer.bindDescriptorSets(
        vk::PipelineBindPoint::eGraphics,mPipelineLayout,
        1, 1, &gRuntimeGlobalContext.getRenderResource()->mCameraBufferResource->mDescriptorSet,
        0, nullptr);
    for (const auto& iter : gRuntimeGlobalContext.getRenderResource()->mModelRenderResources)
    {
        // 更新模型位置
        gRuntimeGlobalContext.getRHI()->mCommandBuffer.bindDescriptorSets(
            vk::PipelineBindPoint::eGraphics, mPipelineLayout,
            0, 1, &gRuntimeGlobalContext.getRenderResource()->mObjectBufferResources[iter.first]->mDescriptorSet,
            0, nullptr);

        // 绑定相关数据和绘制
        for (const auto& resource : iter.second)
        {
            gRuntimeGlobalContext.getRHI()->mCommandBuffer.bindDescriptorSets(vk::PipelineBindPoint::eGraphics,mPipelineLayout,
                2, 1, &resource.mTextureResource.lock()->mTextureBufferResource.mDescriptorSet,
                0, nullptr);
            gRuntimeGlobalContext.getRHI()->mCommandBuffer.bindVertexBuffers(0, 1, &resource.mMeshResource.lock()->mMeshBufferResource.mVertexResource->mBuffer, &offset);
            gRuntimeGlobalContext.getRHI()->mCommandBuffer.bindIndexBuffer(resource.mMeshResource.lock()->mMeshBufferResource.mIndexResource->mBuffer, offset, vk::IndexType::eUint32);
            gRuntimeGlobalContext.getRHI()->mCommandBuffer.drawIndexed(resource.mMeshResource.lock()->mMeshBufferResource.mIndexResource->mIndexCount, 1, 0, 0, 0);
        }

    }
    //gRuntimeGlobalContext.getRHI()->mCommandBuffer.endRenderPass();
}

vk::RenderPass MainCameraPass::getRenderPass()
{
    return mFrame.mRenderPass;
}

void MainCameraPass::setupAttachments()
{
    FrameBufferAttachment depthFrameBufferAttachment;
    depthFrameBufferAttachment.mFormat = vk::Format::eD24UnormS8Uint;
    VulkanUtil::createImage(
        gRuntimeGlobalContext.getRHI()->mSwapchainSupportDetails.mExtent2D.width,
        gRuntimeGlobalContext.getRHI()->mSwapchainSupportDetails.mExtent2D.height,
        depthFrameBufferAttachment.mFormat,
        vk::ImageTiling::eOptimal,
        vk::ImageUsageFlagBits::eDepthStencilAttachment,
        vk::MemoryPropertyFlagBits::eDeviceLocal,
        depthFrameBufferAttachment.mImage,
        depthFrameBufferAttachment.mMemory);
    depthFrameBufferAttachment.mImageView = VulkanUtil::createImageView(
        vk::ImageAspectFlagBits::eDepth,
        depthFrameBufferAttachment.mFormat,
        depthFrameBufferAttachment.mImage);

    mFrame.mAttachments.insert(std::make_pair(ATTACHMENT_TYPE::TYPE_DEPTH, depthFrameBufferAttachment));

    VulkanUtil::transitionImageLayout(
        depthFrameBufferAttachment.mImage,
        depthFrameBufferAttachment.mFormat,
        vk::ImageLayout::eUndefined,
        vk::ImageLayout::eDepthStencilAttachmentOptimal,
        1);
}

void MainCameraPass::setupRenderPass()
{
    std::array<vk::AttachmentDescription, 2> attachmentDesces;

    vk::AttachmentDescription& colorAttachmentDesc = attachmentDesces[0];
    colorAttachmentDesc.format = gRuntimeGlobalContext.getRHI()->mSwapchainSupportDetails.mFormat.format;
    colorAttachmentDesc.samples = vk::SampleCountFlagBits::e1;
    colorAttachmentDesc.loadOp = vk::AttachmentLoadOp::eClear;
    colorAttachmentDesc.storeOp = vk::AttachmentStoreOp::eStore;
    colorAttachmentDesc.initialLayout = vk::ImageLayout::eUndefined;
    colorAttachmentDesc.finalLayout = vk::ImageLayout::ePresentSrcKHR;
    colorAttachmentDesc.stencilLoadOp = vk::AttachmentLoadOp::eDontCare;
    colorAttachmentDesc.stencilStoreOp = vk::AttachmentStoreOp::eDontCare;

    vk::AttachmentDescription& depthAttachmentDesc = attachmentDesces[1];
    depthAttachmentDesc.format = mFrame.mAttachments[ATTACHMENT_TYPE::TYPE_DEPTH].mFormat;
    depthAttachmentDesc.samples = vk::SampleCountFlagBits::e1;
    depthAttachmentDesc.loadOp = vk::AttachmentLoadOp::eClear;
    depthAttachmentDesc.storeOp = vk::AttachmentStoreOp::eDontCare;
    depthAttachmentDesc.initialLayout = vk::ImageLayout::eUndefined;
    depthAttachmentDesc.finalLayout = vk::ImageLayout::eDepthStencilAttachmentOptimal;
    depthAttachmentDesc.stencilLoadOp = vk::AttachmentLoadOp::eDontCare;
    depthAttachmentDesc.stencilStoreOp = vk::AttachmentStoreOp::eDontCare;

    vk::AttachmentReference attachmentRef[2];
    attachmentRef[0].attachment = 0;
    attachmentRef[0].layout = vk::ImageLayout::eColorAttachmentOptimal; // 过程中的布局,这也是为什么subpass可以多个
    attachmentRef[1].attachment = 1;
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

void MainCameraPass::setupDescriptorSetLayout()
{
    mDescSetLayouts.resize(3);
    mDescSetLayouts[DESCRIPTOR_TYPE::DESCRIPTOR_TYPE_OBJECTUNIFORM] = gRuntimeGlobalContext.getRenderResource()->getDescriptorSetLayout(DESCRIPTOR_TYPE::DESCRIPTOR_TYPE_OBJECTUNIFORM);
    mDescSetLayouts[DESCRIPTOR_TYPE::DESCRIPTOR_TYPE_CAMERAUNIFORM] = gRuntimeGlobalContext.getRenderResource()->getDescriptorSetLayout(DESCRIPTOR_TYPE::DESCRIPTOR_TYPE_CAMERAUNIFORM);
    mDescSetLayouts[DESCRIPTOR_TYPE::DESCRIPTOR_TYPE_SAMPLE] = gRuntimeGlobalContext.getRenderResource()->getDescriptorSetLayout(DESCRIPTOR_TYPE::DESCRIPTOR_TYPE_SAMPLE);
}

void MainCameraPass::setupPipelines()
{
    // vertex Descriptions
    vk::PipelineVertexInputStateCreateInfo vertexInfo;
    auto VertexAttributeDescriptions = VertexResource::getInputAttributes({ VertexAttribute::VA_Position,VertexAttribute::VA_Color,VertexAttribute::VA_UV0 });
    auto VertexBindingDescriptions = VertexResource::getBindingDescription({ VertexAttribute::VA_Position,VertexAttribute::VA_Color,VertexAttribute::VA_UV0 });
    vertexInfo.vertexAttributeDescriptionCount = VertexAttributeDescriptions.size();
    vertexInfo.vertexBindingDescriptionCount = VertexBindingDescriptions.size();
    vertexInfo.pVertexAttributeDescriptions = VertexAttributeDescriptions.data();
    vertexInfo.pVertexBindingDescriptions = VertexBindingDescriptions.data();

    // input assembler
    vk::PipelineInputAssemblyStateCreateInfo inputInfo;
    inputInfo.primitiveRestartEnable = false;
    inputInfo.topology = vk::PrimitiveTopology::eTriangleList;

    // shaderStatus
    std::array<vk::PipelineShaderStageCreateInfo, 2> shaderStatus;
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

    // viewport
    vk::PipelineViewportStateCreateInfo viewportInfo;
    vk::Rect2D scissors;
    scissors.offset = 0;
    scissors.extent = gRuntimeGlobalContext.getRHI()->mSwapchainSupportDetails.mExtent2D;
    vk::Viewport viewport;
    viewport.height = (float)gRuntimeGlobalContext.getRHI()->mSwapchainSupportDetails.mExtent2D.height;
    viewport.width = (float)gRuntimeGlobalContext.getRHI()->mSwapchainSupportDetails.mExtent2D.width;
    viewport.maxDepth = 1.f;
    viewport.minDepth = 0.f;
    viewport.x = 0.f;
    viewport.y = 0.f;
    viewportInfo.pScissors = &scissors;
    viewportInfo.pViewports = &viewport;
    viewportInfo.scissorCount = 1;
    viewportInfo.viewportCount = 1;

    // rasterization
    vk::PipelineRasterizationStateCreateInfo rasterInfo;
    rasterInfo.depthBiasEnable = false;
    rasterInfo.depthClampEnable = false;
    rasterInfo.rasterizerDiscardEnable = false;
    rasterInfo.cullMode = vk::CullModeFlagBits::eBack;
    rasterInfo.frontFace = vk::FrontFace::eCounterClockwise;
    rasterInfo.lineWidth = 1.f;
    rasterInfo.polygonMode = vk::PolygonMode::eFill;

    // multi sample
    vk::PipelineMultisampleStateCreateInfo multisampleInfo;
    multisampleInfo.alphaToCoverageEnable = false;
    multisampleInfo.alphaToOneEnable = false;
    multisampleInfo.sampleShadingEnable = false;

    // deptp/mask test
    vk::PipelineDepthStencilStateCreateInfo depthInfo;
    depthInfo.stencilTestEnable = false;
    depthInfo.depthTestEnable = true;
    depthInfo.depthWriteEnable = true;
    depthInfo.depthCompareOp = vk::CompareOp::eLess;

    // colorBlending
    vk::PipelineColorBlendAttachmentState colorBlendAttachmentState;
    colorBlendAttachmentState.blendEnable = false;
    colorBlendAttachmentState.colorWriteMask =
        vk::ColorComponentFlagBits::eR |
        vk::ColorComponentFlagBits::eG |
        vk::ColorComponentFlagBits::eB |
        vk::ColorComponentFlagBits::eA;

    vk::PipelineColorBlendStateCreateInfo blendInfo;
    blendInfo.attachmentCount = 1;
    blendInfo.pAttachments = &colorBlendAttachmentState;
    blendInfo.blendConstants[0] = 0.f;
    blendInfo.blendConstants[1] = 0.f;
    blendInfo.blendConstants[2] = 0.f;
    blendInfo.blendConstants[3] = 0.f;
    blendInfo.logicOpEnable = false;
    blendInfo.logicOp = vk::LogicOp::eCopy;

    // dynamic state

    // pipeline layout
    vk::PipelineLayoutCreateInfo pipelineLayoutInfo;
    pipelineLayoutInfo.setLayoutCount = (uint32_t)mDescSetLayouts.size();
    pipelineLayoutInfo.pSetLayouts = mDescSetLayouts.data();
    mPipelineLayout = gRuntimeGlobalContext.getRHI()->mDevice.createPipelineLayout(pipelineLayoutInfo);

    // pipeline RenderPass

    vk::GraphicsPipelineCreateInfo info;
    info.pVertexInputState = &vertexInfo;
    info.pInputAssemblyState = &inputInfo;
    info.stageCount = (uint32_t)shaderStatus.size();
    info.pStages = shaderStatus.data();
    info.pViewportState = &viewportInfo;
    info.pRasterizationState = &rasterInfo;
    info.pMultisampleState = &multisampleInfo;
    info.pDepthStencilState = &depthInfo;
    info.pColorBlendState = &blendInfo;
    info.pDynamicState = nullptr;
    info.layout = mPipelineLayout;
    info.renderPass = mFrame.mRenderPass;
    mPipeline = gRuntimeGlobalContext.getRHI()->mDevice.createGraphicsPipeline(VK_NULL_HANDLE, info).value;
    CHECK_NULL(mPipeline);

    gRuntimeGlobalContext.getRHI()->mDevice.destroyShaderModule(vertShaderModule);
    gRuntimeGlobalContext.getRHI()->mDevice.destroyShaderModule(fragShaderModule);
}

void MainCameraPass::setupDescriptorSet()
{
}

void MainCameraPass::setupSwapchainFramebuffers()
{
    mFrame.mFramebuffer.resize(gRuntimeGlobalContext.getRHI()->mSwapchainSupportDetails.mImageCount);

    for (uint32_t i = 0; i < gRuntimeGlobalContext.getRHI()->mSwapchainSupportDetails.mImageCount; i++)
    {
        std::array<vk::ImageView, 2> attachments = {
            gRuntimeGlobalContext.getRHI()->mSwapchainImageViews[i],
            mFrame.mAttachments[ATTACHMENT_TYPE::TYPE_DEPTH].mImageView
        };

        vk::FramebufferCreateInfo info;
        info.renderPass = mFrame.mRenderPass;
        info.pAttachments = attachments.data();
        info.attachmentCount = (uint32_t)attachments.size();
        info.height = gRuntimeGlobalContext.getRHI()->mSwapchainSupportDetails.mExtent2D.height;
        info.width = gRuntimeGlobalContext.getRHI()->mSwapchainSupportDetails.mExtent2D.width;
        info.layers = 1;

        mFrame.mFramebuffer[i] = gRuntimeGlobalContext.getRHI()->mDevice.createFramebuffer(info);
        CHECK_NULL(mFrame.mFramebuffer[i]);
    }
}
