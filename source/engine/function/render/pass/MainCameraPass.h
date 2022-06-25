#pragma once
#include <unordered_map>
#include <vulkan/vulkan.hpp>
#include "function/render/RenderResource/RenderData.h"

enum class ATTACHMENT_TYPE : uint8_t
{
	TYPE_DEPTH
};

struct FrameBufferAttachment
{
public:
	vk::Image mImage;
	vk::ImageView mImageView;
	vk::DeviceMemory mMemory;
	vk::Format mFormat;
};

struct Frame
{
public:
	std::vector<vk::Framebuffer> mFramebuffer;
	vk::RenderPass mRenderPass;
	std::unordered_map<ATTACHMENT_TYPE, FrameBufferAttachment> mAttachments;
public:
	std::vector<FrameBufferAttachment> getAttachments()
	{
		std::vector<FrameBufferAttachment> result;
		for (const auto& attachment : mAttachments)
		{
			result.push_back(attachment.second);
		}

		return result;
	}
};

class MainCameraPass
{
public:
	~MainCameraPass();
	void initialize();
	void drawPass();

private:
	std::vector<vk::DescriptorSetLayout> mDescSetLayouts;
	vk::PipelineLayout mPipelineLayout;
	vk::Pipeline mPipeline;
	std::vector<vk::DescriptorSet> mDescriptorSets;
	Frame mFrame;

	CameraBufferData cameraData;

private:
	void setupAttachments();
	void setupRenderPass();
	void setupDescriptorSetLayout();
	void setupPipelines();
	void setupDescriptorSet();
	void setupSwapchainFramebuffers();
};
