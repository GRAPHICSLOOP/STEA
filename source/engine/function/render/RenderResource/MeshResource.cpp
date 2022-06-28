#include "MeshResource.h"
#include "function/global/RuntimeGlobalContext.h"

MeshResource::MeshResource(const std::vector<VertexBufferData>& vertices, const std::vector<uint32_t>& indices)
{
	gRuntimeGlobalContext.getRenderResource()->createVertexBuffer(mMeshBufferResource, vertices.data(), (uint32_t)vertices.size());
	mMeshBufferResource.mIndexResource = IndexResource::create(indices.data(), (uint32_t)indices.size(),sizeof(uint32_t));
}

MeshResource::~MeshResource()
{
	gRuntimeGlobalContext.getRHI()->mDevice.destroyBuffer(mMeshBufferResource.mVertexBuffer);
	gRuntimeGlobalContext.getRHI()->mDevice.freeMemory(mMeshBufferResource.mVertexBufferMemory);
}
