#include "MeshResource.h"
#include "function/global/RuntimeGlobalContext.h"

MeshResource::MeshResource(const std::vector<VertexBufferData>& vertices, const std::vector<uint32_t>& indices)
{
	mMeshBufferResource.mVertexResource = VertexResource::create(vertices.data(), vertices.size(), {VertexAttribute::VA_Position,VertexAttribute::VA_Color,VertexAttribute::VA_UV0});
	mMeshBufferResource.mIndexResource = IndexResource::create(indices.data(), (uint32_t)indices.size(),sizeof(uint32_t));
}

MeshResource::~MeshResource()
{

}
