#pragma once
#include <vector>
#include "RenderData.h"
#include "IndexResource.h"

struct MeshBufferResource
{
public:
	uint32_t mVertexCount;
	uint32_t mIndexCount;
	vk::Buffer mVertexBuffer;
	vk::DeviceMemory mVertexBufferMemory;
	//std::shared_ptr<IndexResource> mIndexResource;
	vk::Buffer mIndexBuffer;
	vk::DeviceMemory mIndexBufferMemory;
};

class MeshResource
{
public:
	MeshResource(const std::vector<VertexBufferData>& vertices, const std::vector<uint32_t>& indices);
	~MeshResource();

public:
	MeshBufferResource mMeshBufferResource;
};


