#pragma once
#include "RenderPassBase.h"
#include <unordered_map>
#include "function/render/RenderResource/RenderData.h"

struct FrameBufferAttachment
{
public:
	vk::Image mImage;
	vk::ImageView mImageView;
	vk::DeviceMemory mMemory;
	vk::Format mFormat;
};

enum class ATTACHMENT_TYPE : uint8_t
{
	Depth,
	Color,
	Normal
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

class MainCameraPass : public RenderPassBase
{
public:
	void drawPass() override;
};
