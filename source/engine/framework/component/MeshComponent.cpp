#include "MeshComponent.h"
#include "function/global/RuntimeGlobalContext.h"
#include "../object/Object.h"
#include "TransformComponent.h"

MeshComponent::MeshComponent(std::shared_ptr<class Object> obj)
	:Component(obj)
{

}

void MeshComponent::tick(float delta)
{
	TransformComponent* tran = mObject.lock()->mRootComponent->getComponent<TransformComponent>();
	if (!tran)
		return;

	ObjectBufferData data;
	data.mModel = tran->getModelMatrix();
	gRuntimeGlobalContext.getRenderResource()->mObjectBufferDatas[mObject.lock()->getID()] = data;
}

void MeshComponent::TempLoadMeshAsset(std::string name, std::string path)
{
	std::shared_ptr<MeshAsset> asset = std::make_shared<MeshAsset>();
	asset->initialize(name, path);
	setMeshAsset(asset);
	gRuntimeGlobalContext.getRenderResource()->mModelRenderResource.insert(std::make_pair(mObject.lock()->getID(),asset->getMeshResource()));
}

void MeshComponent::setMeshAsset(std::shared_ptr<MeshAsset> asset)
{
	mMeshAsset = asset;
}
