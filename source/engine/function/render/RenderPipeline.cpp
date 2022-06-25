#include "RenderPipeline.h"

void RenderPipeline::initialize()
{
	mCameraPass = std::make_shared<MainCameraPass>();
	mCameraPass->initialize();
}

void RenderPipeline::draw()
{
	mCameraPass->drawPass();
}
