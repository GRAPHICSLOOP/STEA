#include "RenderPipeline.h"
#include "function/global/RuntimeGlobalContext.h"

void RenderPipeline::initialize()
{
	mCameraPass = std::make_shared<MainCameraPass>();
	mCameraPass->initialize();

	mUIPass = std::make_shared<UIPass>();
	mUIPass->initialize(UIPassConfigParam(mCameraPass->getRenderPass()));
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
    passBegineInfo.renderPass = mCameraPass->getRenderPass();
    passBegineInfo.framebuffer = mCameraPass->getFrameBuffer(gRuntimeGlobalContext.getRHI()->getNextImageIndex());
    vk::Rect2D area;
    area.offset = 0;
    area.extent = gRuntimeGlobalContext.getRHI()->mSwapchainSupportDetails.mExtent2D;
    passBegineInfo.setRenderArea(area);

    std::array<vk::ClearValue, 2> clearValues;
    clearValues[0].color = std::array<float, 4>{1, 1, 1, 1.f};
    clearValues[1].depthStencil = 1.f;
    passBegineInfo.pClearValues = clearValues.data();
    passBegineInfo.clearValueCount = 2;


    gRuntimeGlobalContext.getRHI()->mCommandBuffer.beginRenderPass(passBegineInfo, vk::SubpassContents::eInline);

}

void RenderPipeline::endDraw()
{
    gRuntimeGlobalContext.getRHI()->mCommandBuffer.endRenderPass();
}
