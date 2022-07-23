#include "RenderPassBase.h"
#include "../../global/RuntimeGlobalContext.h"
#include "core/base/macro.h"

RenderPassBase::RenderPassBase()
{
    // input assembler
    mInputInfo.primitiveRestartEnable = false;
    mInputInfo.topology = vk::PrimitiveTopology::eTriangleList;

    // mViewport
    mScissors.offset = 0;
    mScissors.extent = gRuntimeGlobalContext.getRHI()->mSwapchainSupportDetails.mExtent2D;

    mViewport.height = (float)gRuntimeGlobalContext.getRHI()->mSwapchainSupportDetails.mExtent2D.height;
    mViewport.width = (float)gRuntimeGlobalContext.getRHI()->mSwapchainSupportDetails.mExtent2D.width;
    mViewport.maxDepth = 1.f;
    mViewport.minDepth = 0.f;
    mViewport.x = 0.f;
    mViewport.y = 0.f;
    mViewportInfo.pScissors = &mScissors;
    mViewportInfo.pViewports = &mViewport;
    mViewportInfo.scissorCount = 1;
    mViewportInfo.viewportCount = 1;

    // rasterization
    mRasterInfo.depthBiasEnable = false;
    mRasterInfo.depthClampEnable = false;
    mRasterInfo.rasterizerDiscardEnable = false;
    mRasterInfo.cullMode = vk::CullModeFlagBits::eBack;
    mRasterInfo.frontFace = vk::FrontFace::eCounterClockwise;
    mRasterInfo.lineWidth = 1.f;
    mRasterInfo.polygonMode = vk::PolygonMode::eFill;

    // multi sample
    mMultisampleInfo.alphaToCoverageEnable = false;
    mMultisampleInfo.alphaToOneEnable = false;
    mMultisampleInfo.sampleShadingEnable = false;

    // deptp/mask test
    mDepthInfo.stencilTestEnable = false;
    mDepthInfo.depthTestEnable = true;
    mDepthInfo.depthWriteEnable = true;
    mDepthInfo.depthCompareOp = vk::CompareOp::eLess;

    // colorBlending
    for (uint32_t i = 0; i < 8; ++i)
    {
        mColorBlendAttachmentState[i].colorWriteMask =
            vk::ColorComponentFlagBits::eR |
            vk::ColorComponentFlagBits::eG |
            vk::ColorComponentFlagBits::eB |
            vk::ColorComponentFlagBits::eA;

        mColorBlendAttachmentState[i].blendEnable = VK_FALSE;
        mColorBlendAttachmentState[i].srcColorBlendFactor = vk::BlendFactor::eOne;
        mColorBlendAttachmentState[i].dstColorBlendFactor = vk::BlendFactor::eZero;
        mColorBlendAttachmentState[i].colorBlendOp = vk::BlendOp::eAdd;
        mColorBlendAttachmentState[i].srcAlphaBlendFactor = vk::BlendFactor::eOne;
        mColorBlendAttachmentState[i].dstAlphaBlendFactor = vk::BlendFactor::eOne;
        mColorBlendAttachmentState[i].alphaBlendOp = vk::BlendOp::eAdd;
    }

    

}

RenderPassBase::~RenderPassBase()
{
    gRuntimeGlobalContext.getRHI()->mDevice.destroyPipeline(mPipeline);
    gRuntimeGlobalContext.getRHI()->mDevice.destroyPipelineLayout(mPipelineLayout);
}

vk::Pipeline RenderPassBase::initialize(
    const vk::PipelineVertexInputStateCreateInfo& vertexInfo,
    const Shader* shader,
    const vk::RenderPass renderPass)
{
    mDescSetLayout = shader->mDescriptorSetLayouts;

    // shaderStatus
    std::vector<vk::PipelineShaderStageCreateInfo> shaderStatus(shader->mShaderModule.size());
    for (uint32_t i = 0; i < shader->mShaderModule.size(); i++)
    {
        vk::PipelineShaderStageCreateInfo& info = shaderStatus[i];
        info.module = shader->mShaderModule[i].mShaderModule;
        info.pName = "main";
        info.stage = shader->mShaderModule[i].mStage;
    }
    mShader = shader;

    // pipeline layout
    vk::PipelineLayoutCreateInfo pipelineLayoutInfo;
    pipelineLayoutInfo.setLayoutCount = (uint32_t)mDescSetLayout.size();
    pipelineLayoutInfo.pSetLayouts = mDescSetLayout.data();
    pipelineLayoutInfo.pPushConstantRanges = mPushRange.data();
    pipelineLayoutInfo.pushConstantRangeCount = (uint32_t)mPushRange.size();
    mPipelineLayout = gRuntimeGlobalContext.getRHI()->mDevice.createPipelineLayout(pipelineLayoutInfo);

    mBlendInfo.attachmentCount = mColorBlendAttachmentCount;
    mBlendInfo.pAttachments = mColorBlendAttachmentState.data();
    mBlendInfo.blendConstants[0] = 0.f;
    mBlendInfo.blendConstants[1] = 0.f;
    mBlendInfo.blendConstants[2] = 0.f;
    mBlendInfo.blendConstants[3] = 0.f;
    mBlendInfo.logicOpEnable = false;
    mBlendInfo.logicOp = vk::LogicOp::eCopy;

    vk::GraphicsPipelineCreateInfo info;
    info.pVertexInputState = &vertexInfo;
    info.pInputAssemblyState = &mInputInfo;
    info.stageCount = (uint32_t)shaderStatus.size();
    info.pStages = shaderStatus.data();
    info.pViewportState = &mViewportInfo;
    info.pRasterizationState = &mRasterInfo;
    info.pMultisampleState = &mMultisampleInfo;
    info.pDepthStencilState = &mDepthInfo;
    info.pColorBlendState = &mBlendInfo;
    info.pDynamicState = nullptr;
    info.layout = mPipelineLayout;
    info.renderPass = renderPass;
    info.subpass = mSubpassIndex;
    mPipeline = gRuntimeGlobalContext.getRHI()->mDevice.createGraphicsPipeline(VK_NULL_HANDLE, info).value;
    CHECK_NULL(mPipeline);

    //for (uint32_t i = 0; i < shader->mShaderModule.size(); i++)
    //{
    //    gRuntimeGlobalContext.getRHI()->mDevice.destroyShaderModule(shader->mShaderModule[i].mShaderModule);
    //}

    return mPipeline;
}

void RenderPassBase::drawPass(vk::CommandBuffer cmdBuffer)
{

}
