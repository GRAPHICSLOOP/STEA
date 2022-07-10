#pragma once
#include "RenderPassBase.h"
#include "../RenderResource/VertexResource.h"
#include "../RenderResource/IndexResource.h"

class PostProcessPass : public RenderPassBase
{
public:
	virtual vk::Pipeline initialize(
		const vk::PipelineVertexInputStateCreateInfo& vertexInfo,
		const std::vector<vk::PipelineShaderStageCreateInfo>& shaderStatus,
		std::vector<vk::DescriptorSetLayout> descriptorSetLayout,
		const vk::RenderPass renderPass) override;

	virtual void drawPass(vk::CommandBuffer cmdBuffer) override;

	void createDescriptorSet(Frame& frame);

private:
	std::shared_ptr<VertexResource> mQuadVertexResource;
	std::shared_ptr<IndexResource> mQuadIndexResource;
	vk::DescriptorSet mDescSet;
};

