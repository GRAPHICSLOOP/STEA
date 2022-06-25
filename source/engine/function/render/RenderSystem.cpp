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

}
