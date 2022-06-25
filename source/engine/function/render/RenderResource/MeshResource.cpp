#include "MeshResource.h"
#include "function/global/RuntimeGlobalContext.h"

MeshResource::MeshResource(const std::vector<VertexBufferData>& vertices, const std::vector<uint32_t>& indices)
{
	gRuntimeGlobalContext.getRenderResource()->createVertexBuffer(mMeshBufferResource, vertices.data(), (uint32_t)vertices.size());
	gRuntimeGlobalContext.getRenderResource()->createIndexBuffer(mMeshBufferResource, indices.data(), (uint32_t)indices.size());
}

MeshResource::~MeshResource()
{
	gRuntimeGlobalContext.getRHI()->mDevice.destroyBuffer(mMeshBufferResource.mVertexBuffer);
	gRuntimeGlobalContext.getRHI()->mDevice.destroyBuffer(mMeshBufferResource.mIndexBuffer);
	gRuntimeGlobalContext.getRHI()->mDevice.freeMemory(mMeshBufferResource.mVertexBufferMemory);
	gRuntimeGlobalContext.getRHI()->mDevice.freeMemory(mMeshBufferResource.mIndexBufferMemory);
}
