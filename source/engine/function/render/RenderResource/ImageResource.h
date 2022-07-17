#pragma once
#include <vulkan/vulkan.hpp>

struct ImageBufferResource
{
public:
	vk::Format mFormat;
	vk::Image mImage;
	vk::DeviceMemory mMemory;
	vk::DescriptorImageInfo mImageInfo;
	vk::DescriptorSet mDescriptorSet;
};


class ImageResource
{
public:
	ImageResource();
	~ImageResource();

	static void getPixelInfo(const vk::Format pixelFormat,const uint32_t width,const uint32_t height, size_t& pixelSize);
	static void setImageLayout(vk::CommandBuffer cmdBuffer, ImageBufferResource* image, vk::ImageLayout dstLayout, const vk::ImageSubresourceRange& subresourceRange);
	static void setImageLayout(vk::CommandBuffer cmdBuffer, vk::Image image, vk::ImageLayout srcLayout, vk::ImageLayout dstLayout, const vk::ImageSubresourceRange& subresourceRange);
	static void setImageBarrierInfo(vk::AccessFlags& accessFlags, vk::ImageLayout targetLayout, vk::PipelineStageFlags& stages);

	static std::shared_ptr<ImageResource> createTextureResource(
		uint32_t width,
		uint32_t height,
		vk::ImageUsageFlags usage,
		void* pixels,
		vk::Format pixelFormat,
		bool miplevel,
		vk::DescriptorSetLayout setLayout);

	static std::shared_ptr<ImageResource> createAttachment(
		uint32_t width,
		uint32_t height,
		vk::ImageUsageFlags usage,
		vk::ImageAspectFlags aspectFlags,
		vk::Format pixelFormat,
		vk::DescriptorSetLayout setLayout);

public:
	ImageBufferResource mImageBufferResource;
	
private:
	void createDescriptorSet(vk::DescriptorSetLayout setLayout);
};


