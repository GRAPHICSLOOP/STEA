#pragma once
#include <vulkan/vulkan.hpp>
#include "../RenderResource/RenderData.h"

class RenderPassBase
{
public:
	RenderPassBase();
	virtual ~RenderPassBase();

	virtual vk::Pipeline initialize(
		const vk::PipelineVertexInputStateCreateInfo& vertexInfo,
		const std::vector<vk::PipelineShaderStageCreateInfo>& shaderStatus,
		std::vector<vk::DescriptorSetLayout> descriptorSetLayout,
		const vk::RenderPass renderPass);

	virtual void drawPass(vk::CommandBuffer cmdBuffer);

public:
	vk::PipelineInputAssemblyStateCreateInfo mInputInfo;
	vk::PipelineViewportStateCreateInfo mViewportInfo;
	vk::Rect2D mScissors;
	vk::Viewport mViewport;
	vk::PipelineRasterizationStateCreateInfo mRasterInfo;
	vk::PipelineMultisampleStateCreateInfo mMultisampleInfo;
	vk::PipelineDepthStencilStateCreateInfo mDepthInfo;
	std::array < vk::PipelineColorBlendAttachmentState, 8> mColorBlendAttachmentState;
	vk::PipelineColorBlendStateCreateInfo mBlendInfo;
	std::vector<vk::PushConstantRange> mPushRange;
	uint32_t mColorBlendAttachmentCount = 1;
	uint32_t mSubpassIndex = 0;

protected:
	vk::PipelineLayout mPipelineLayout;
	vk::Pipeline mPipeline;
	std::vector<vk::DescriptorSetLayout> mDescSetLayout;

};

