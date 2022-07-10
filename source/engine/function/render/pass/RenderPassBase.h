#pragma once
#include <vulkan/vulkan.hpp>


class RenderPassBase
{
public:
	RenderPassBase();
	virtual ~RenderPassBase();

	virtual vk::Pipeline initialize(
		const vk::PipelineVertexInputStateCreateInfo& vertexInfo,
		const std::vector<vk::PipelineShaderStageCreateInfo>& shaderStatus,
		const vk::PipelineLayout pipelineLayout,
		const vk::RenderPass renderPass);

	virtual void drawPass();

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
	uint32_t mColorBlendAttachmentCount = 1;

protected:
	vk::PipelineLayout mPipelineLayout;
	vk::Pipeline mPipeline;
};

