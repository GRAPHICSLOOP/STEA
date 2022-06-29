#pragma once
#include <vector>
#include "RenderData.h"
#include "IndexResource.h"
#include "VertexResource.h"

struct MeshBufferResource
{
public:
	std::shared_ptr<VertexResource> mVertexResource;
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


