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

	cmdBuffer.bindDescriptorSets(vk::PipelineBindPoint::eGraphics, mPipelineLayout, 0, 1, &mDescriptorSet, 0, nullptr);
	cmdBuffer.bindDescriptorSets(vk::PipelineBindPoint::eGraphics, mPipelineLayout, 1, 1, &gRuntimeGlobalContext.getRenderResource()->mQuadUniformResource->mDescriptorSet, 0, nullptr);
	cmdBuffer.bindVertexBuffers(0, 1, &mQuadVertexResource->mBuffer, &offset);
	cmdBuffer.bindIndexBuffer(mQuadIndexResource->mBuffer, offset, vk::IndexType::eUint32);
	cmdBuffer.drawIndexed(mQuadIndexResource->mIndexCount, 1, 0, 0, 0);

}

void PostProcessPass::setDescriptorSet(vk::DescriptorSet descriptorSet)
{
	mDescriptorSet = descriptorSet;
}
