#include "VertexResource.h"
#include "../VulkanUtil.h"
#include "../../global/RuntimeGlobalContext.h"

uint32_t VertexAttributeToSize(VertexAttribute va)
{
	// count * sizeof(float)
	if (va == VertexAttribute::VA_Position) {
		return 3 * sizeof(float);
	}
	else if (va == VertexAttribute::VA_UV0) {
		return 2 * sizeof(float);
	}
	else if (va == VertexAttribute::VA_UV1) {
		return 2 * sizeof(float);
	}
	else if (va == VertexAttribute::VA_Normal) {
		return 3 * sizeof(float);
	}
	else if (va == VertexAttribute::VA_Tangent) {
		return 4 * sizeof(float);
	}
	else if (va == VertexAttribute::VA_Color) {
		return 3 * sizeof(float);
	}
	else if (va == VertexAttribute::VA_SkinWeight) {
		return 4 * sizeof(float);
	}
	else if (va == VertexAttribute::VA_SkinIndex) {
		return 4 * sizeof(float);
	}
	else if (va == VertexAttribute::VA_Custom0 ||
		va == VertexAttribute::VA_Custom1 ||
		va == VertexAttribute::VA_Custom2 ||
		va == VertexAttribute::VA_Custom3
		)
	{
		return 4 * sizeof(float);
	}

	return 0;
}

vk::Format VertexAttributeToFormat(VertexAttribute va)
{
	vk::Format format = vk::Format::eR32G32B32Sfloat;
	if (va == VertexAttribute::VA_Position) {
		format = vk::Format::eR32G32B32Sfloat;
	}
	else if (va == VertexAttribute::VA_UV0) {
		format = vk::Format::eR32G32Sfloat;
	}
	else if (va == VertexAttribute::VA_UV1) {
		format = vk::Format::eR32G32Sfloat;
	}
	else if (va == VertexAttribute::VA_Normal) {
		format = vk::Format::eR32G32B32Sfloat;
	}
	else if (va == VertexAttribute::VA_Tangent) {
		format = vk::Format::eR32G32B32A32Sfloat;
	}
	else if (va == VertexAttribute::VA_Color) {
		format = vk::Format::eR32G32B32Sfloat;
	}
	else if (va == VertexAttribute::VA_SkinWeight) {
		format = vk::Format::eR32G32B32Sfloat;
	}
	else if (va == VertexAttribute::VA_SkinIndex) {
		format = vk::Format::eR32G32B32Sfloat;
	}
	else if (va == VertexAttribute::VA_Custom0 ||
		va == VertexAttribute::VA_Custom1 ||
		va == VertexAttribute::VA_Custom2 ||
		va == VertexAttribute::VA_Custom3
		)
	{
		format = vk::Format::eR32G32B32A32Sfloat;
	}
	return format;
}

VertexResource::~VertexResource()
{
	gRuntimeGlobalContext.getRHI()->mDevice.destroyBuffer(mBuffer);
	gRuntimeGlobalContext.getRHI()->mDevice.freeMemory(mBufferMemory);
}

std::shared_ptr<VertexResource> VertexResource::create(const void* VerticesData, uint32_t count,const std::vector<VertexAttribute>& vaInputs)
{
	struct make_shared_enabler : public VertexResource {};
	std::shared_ptr<VertexResource> vertexResource = std::make_shared< make_shared_enabler>();
	vertexResource->mVertexAttributes = vaInputs;

	vertexResource->mVertexCount = count;
	uint32_t stride = 0;
	for (const auto& va : vaInputs)
	{
		stride += VertexAttributeToSize(va);
	}

	size_t size = count * (size_t)stride;

	vk::Buffer stagingBuffer;
	vk::DeviceMemory stagingBufferMemory;
	VulkanUtil::createBuffer(
		size,
		vk::BufferUsageFlagBits::eTransferSrc,
		vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostCoherent,
		stagingBuffer,
		stagingBufferMemory);


	void* data;
	vkMapMemory(gRuntimeGlobalContext.getRHI()->mDevice, stagingBufferMemory, 0, size, 0, &data);
	memcpy(data, VerticesData, size);
	vkUnmapMemory(gRuntimeGlobalContext.getRHI()->mDevice, stagingBufferMemory);

	VulkanUtil::createBuffer(
		size,
		vk::BufferUsageFlagBits::eTransferDst | vk::BufferUsageFlagBits::eVertexBuffer,
		vk::MemoryPropertyFlagBits::eDeviceLocal,
		vertexResource->mBuffer,
		vertexResource->mBufferMemory);

	VulkanUtil::copyBuffer(stagingBuffer, vertexResource->mBuffer, size);

	gRuntimeGlobalContext.getRHI()->mDevice.destroyBuffer(stagingBuffer);
	gRuntimeGlobalContext.getRHI()->mDevice.freeMemory(stagingBufferMemory);

	return vertexResource;
}

std::vector<vk::VertexInputAttributeDescription> VertexResource::getInputAttributes(const std::vector<VertexAttribute>& vaInputs)
{
	std::vector<vk::VertexInputAttributeDescription> des;
	des.resize(vaInputs.size());
	uint32_t offset = 0;
	for (uint32_t i = 0; i < vaInputs.size(); i++)
	{
		des[i].binding = 0;
		des[i].location = i;
		des[i].format = VertexAttributeToFormat(vaInputs[i]);
		des[i].offset = offset;
		offset += VertexAttributeToSize(vaInputs[i]);
	}

	return des;
}

std::vector<vk::VertexInputBindingDescription> VertexResource::getBindingDescription(const std::vector<VertexAttribute>& vaInputs)
{
	uint32_t stride = 0;
	for (const auto& va : vaInputs)
	{
		stride += VertexAttributeToSize(va);
	}

	std::vector<vk::VertexInputBindingDescription> des;
	des.resize(1);

	des[0].binding = 0;
	des[0].stride = stride;
	des[0].inputRate = vk::VertexInputRate::eVertex;

	return des;
}

std::vector<vk::VertexInputAttributeDescription> VertexResource::getInputAttributes()
{
	return getInputAttributes(mVertexAttributes);
}

std::vector<vk::VertexInputBindingDescription> VertexResource::getBindingDescription()
{
	return getBindingDescription(mVertexAttributes);
}

