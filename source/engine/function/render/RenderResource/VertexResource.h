#pragma once
#include <memory>
#include <vulkan/vulkan.hpp>

enum class VertexAttribute
{
	VA_None = 0,
	VA_Position,
	VA_UV0,
	VA_UV1,
	VA_Normal,
	VA_Tangent,
	VA_Color,
	VA_SkinWeight,
	VA_SkinIndex,
	VA_Custom0,
	VA_Custom1,
	VA_Custom2,
	VA_Custom3,
	VA_Count,
};

uint32_t VertexAttributeToSize(VertexAttribute va);
vk::Format VertexAttributeToFormat(VertexAttribute va);

class VertexResource
{
private:
	VertexResource()
	{

	}
public:
	~VertexResource();

	static std::shared_ptr<VertexResource> create(const void* VerticesData, uint32_t count,const std::vector<VertexAttribute>& vaInputs);
	static std::vector<vk::VertexInputAttributeDescription> getInputAttributes(const std::vector<VertexAttribute>& vaInputs);
	static std::vector<vk::VertexInputBindingDescription> getBindingDescription(const std::vector<VertexAttribute>& vaInputs);
	std::vector<vk::VertexInputAttributeDescription> getInputAttributes();
	std::vector<vk::VertexInputBindingDescription> getBindingDescription();


public:
	uint32_t mVertexCount;
	vk::Buffer mBuffer;
	vk::DeviceMemory mBufferMemory;

private:
	std::vector<VertexAttribute> mVertexAttributes;
};

