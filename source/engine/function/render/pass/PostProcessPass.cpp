#include "PostProcessPass.h"
#include "../../global/RuntimeGlobalContext.h"

vk::Pipeline PostProcessPass::initialize(
	const vk::PipelineVertexInputStateCreateInfo& vertexInfo,
	const Shader* shader,
	const vk::RenderPass renderPass)
{
	RenderPassBase::initialize(vertexInfo, shader, renderPass);

	{
		std::vector<float> vertices =
		{
			-1.0f,  1.0f, 0.0f, 0.0f, 0.0f,
			1.0f,  1.0f, 0.0f, 1.0f, 0.0f,
			1.0f, -1.0f, 0.0f, 1.0f, 1.0f,
			-1.0f, -1.0f, 0.0f, 0.0f, 1.0f
		};

		std::vector<uint32_t> indices =
		{
			 0, 1, 2, 0, 2, 3
		};

		mQuadVertexResource = VertexResource::create(vertices.data(), (uint32_t)vertices.size(), { VertexAttribute::VA_Position,VertexAttribute::VA_UV0 });
		mQuadIndexResource = IndexResource::create(indices.data(), (uint32_t)indices.size(), sizeof(uint32_t));
	}

	return mPipeline;
}

void PostProcessPass::drawPass(vk::CommandBuffer cmdBuffer)
{
	cmdBuffer.bindPipeline(vk::PipelineBindPoint::eGraphics, mPipeline);
	vk::DeviceSize offset = 0;

	cmdBuffer.bindDescriptorSets(vk::PipelineBindPoint::eGraphics, mPipelineLayout, 0, 1, &mDescSet, 0, nullptr);
	cmdBuffer.bindVertexBuffers(0, 1, &mQuadVertexResource->mBuffer, &offset);
	cmdBuffer.bindIndexBuffer(mQuadIndexResource->mBuffer, offset, vk::IndexType::eUint32);
	cmdBuffer.drawIndexed(mQuadIndexResource->mIndexCount, 1, 0, 0, 0);

}

void PostProcessPass::createDescriptorSet(Frame& frame)
{
	vk::DescriptorSetAllocateInfo info;
	info.descriptorPool = gRuntimeGlobalContext.getRHI()->mDescriptorPool;
	info.descriptorSetCount = (uint32_t)mDescSetLayout.size();
	info.pSetLayouts = mDescSetLayout.data();
	mDescSet = gRuntimeGlobalContext.getRHI()->mDevice.allocateDescriptorSets(info)[0];

	vk::WriteDescriptorSet writeSet;
	vk::DescriptorImageInfo imageInfo;

	// colorImageInfo
	imageInfo.imageLayout = vk::ImageLayout::eShaderReadOnlyOptimal;
	imageInfo.imageView = frame.mAttachments[1]->mImageBufferResource.mImageInfo.imageView;
	imageInfo.sampler = VK_NULL_HANDLE;
		
	// 更新描述符
	writeSet.dstArrayElement = 0;
	writeSet.dstBinding = 0;
	writeSet.dstSet = mDescSet;
	writeSet.descriptorType = vk::DescriptorType::eInputAttachment;
	writeSet.descriptorCount = 1;
	writeSet.pBufferInfo = nullptr;
	writeSet.pImageInfo = &imageInfo;

	gRuntimeGlobalContext.getRHI()->mDevice.updateDescriptorSets(writeSet, nullptr);

	// normalImageInfo
	imageInfo.imageLayout = vk::ImageLayout::eShaderReadOnlyOptimal;
	imageInfo.imageView = frame.mAttachments[2]->mImageBufferResource.mImageInfo.imageView;
	imageInfo.sampler = VK_NULL_HANDLE;

	// 更新描述符
	writeSet.dstArrayElement = 0;
	writeSet.dstBinding = 1;
	writeSet.dstSet = mDescSet;
	writeSet.descriptorType = vk::DescriptorType::eInputAttachment;
	writeSet.descriptorCount = 1;
	writeSet.pBufferInfo = nullptr;
	writeSet.pImageInfo = &imageInfo;

	gRuntimeGlobalContext.getRHI()->mDevice.updateDescriptorSets(writeSet, nullptr);

	// depthImageInfo
	imageInfo.imageLayout = vk::ImageLayout::eShaderReadOnlyOptimal;
	imageInfo.imageView = frame.mAttachments[0]->mImageBufferResource.mImageInfo.imageView;
	imageInfo.sampler = VK_NULL_HANDLE;

	// 更新描述符
	writeSet.dstArrayElement = 0;
	writeSet.dstBinding = 2;
	writeSet.dstSet = mDescSet;
	writeSet.descriptorType = vk::DescriptorType::eInputAttachment;
	writeSet.descriptorCount = 1;
	writeSet.pBufferInfo = nullptr;
	writeSet.pImageInfo = &imageInfo;

	gRuntimeGlobalContext.getRHI()->mDevice.updateDescriptorSets(writeSet, nullptr);
}
