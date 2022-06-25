#include "RenderSystem.h"

void RenderSystem::initialize()
{
	mVulkanRHI = std::make_shared<VulkanRHI>();
	mVulkanRHI->initialize();

	mRenderResource = std::make_shared<RenderResource>();
	mRenderResource->initialize();

	mRenderPipeline = std::make_shared<RenderPipeline>();
	mRenderPipeline->initialize();

	mRenderCamera = std::make_shared<RenderCamera>();
}

void RenderSystem::tick()
{
	// 等待上一帧渲染
	mVulkanRHI->mDevice.waitForFences(1, &mVulkanRHI->mFence, VK_TRUE, UINT64_MAX);
	mVulkanRHI->mDevice.resetFences(1, &mVulkanRHI->mFence);

	mRenderResource->updatePerFrameBuffer(mRenderCamera);

	mVulkanRHI->prepareBeforePass();

	mRenderPipeline->draw();

	mVulkanRHI->submitRendering();
}
