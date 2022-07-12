#pragma once
#include "RenderData.h"


class ImageResource
{
public:
	ImageResource();
	~ImageResource();
	void initialize(
		uint32_t width,
		uint32_t height,
		void* pixels,
		PIXEL_FORMAT pixelFormat,
		uint32_t miplevels);

	static void getPixelInfo(const PIXEL_FORMAT pixelFormat,const uint32_t width,const uint32_t height, size_t& pixelSize, vk::Format& vulkanImageFormat);
	static void setImageLayout(vk::CommandBuffer cmdBuffer, ImageBufferResource* image, vk::ImageLayout dstLayout, const vk::ImageSubresourceRange& subresourceRange);
	static void setImageLayout(vk::CommandBuffer cmdBuffer, vk::Image image, vk::ImageLayout srcLayout, vk::ImageLayout dstLayout, const vk::ImageSubresourceRange& subresourceRange);
	static void setImageBarrierInfo(vk::AccessFlags& accessFlags, vk::ImageLayout targetLayout, vk::PipelineStageFlags& stages);

	static std::shared_ptr<ImageResource> createTextureResource(
		uint32_t width,
		uint32_t height,
		vk::ImageUsageFlags usage,
		void* pixels,
		PIXEL_FORMAT pixelFormat,
		bool miplevel);

public:
	ImageBufferResource mImageBufferResource;
	
private:
	vk::Sampler createTextureSampler(uint32_t mipLevels);
	void createDescriptorSet();
};


