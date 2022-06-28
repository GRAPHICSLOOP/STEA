#pragma once
#include <vector>
#include "RenderData.h"
#include "IndexResource.h"

struct MeshBufferResource
{
public:
	uint32_t mVertexCount;
	vk::Buffer mVertexBuffer;
	vk::DeviceMemory mVertexBufferMemory;
	std::shared_ptr<IndexResource> mIndexResource;
};

class MeshResource
{
public:
	MeshResource(const std::vector<VertexBufferData>& vertices, const std::vector<uint32_t>& indices);
	~MeshResource();

public:
	MeshBufferResource mMeshBufferResource;
};


