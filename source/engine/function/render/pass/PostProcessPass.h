#pragma once
#include "RenderPassBase.h"
#include "../RenderResource/VertexResource.h"
#include "../RenderResource/IndexResource.h"

class PostProcessPass : public RenderPassBase
{
public:
	virtual vk::Pipeline initialize(
		const vk::PipelineVertexInputStateCreateInfo& vertexInfo,
		const Shader* shader,
		const vk::RenderPass renderPass) override;

	virtual void drawPass(vk::CommandBuffer cmdBuffer) override;
	void setDescriptorSet(vk::DescriptorSet descriptorSet);

private:
	std::shared_ptr<VertexResource> mQuadVertexResource;
	std::shared_ptr<IndexResource> mQuadIndexResource;
	vk::DescriptorSet mDescriptorSet;
};

