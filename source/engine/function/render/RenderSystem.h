#pragma once
#include <memory>
#include "VulkanRHI.h"
#include "RenderResource.h"
#include "RenderCamera.h"
#include "RenderPipeline.h"

class RenderSystem
{
public:
	void initialize();
	void tick();

public:
	std::shared_ptr<VulkanRHI> mVulkanRHI;
	std::shared_ptr<RenderResource> mRenderResource;
	std::shared_ptr<RenderCamera> mRenderCamera;

private:
	std::shared_ptr<RenderPipeline> mRenderPipeline;
};