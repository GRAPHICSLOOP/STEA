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
	mCameraPass->drawPass();
	mUIPass->drawPass();
	gRuntimeGlobalContext.getRHI()->mCommandBuffer.endRenderPass();
}
