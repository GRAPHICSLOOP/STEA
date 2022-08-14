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
	gRuntimeGlobalContext.getRenderResource()->addObjectBufferResource(mObject.lock()->getID(),&data,sizeof(data));
}

void MeshComponent::TempLoadMeshAsset(std::string name, std::string path)
{
	std::shared_ptr<ModelAsset> asset = std::make_shared<ModelAsset>(name, path);
	asset->loadModel();
	setMeshAsset(asset);
	gRuntimeGlobalContext.getRenderResource()->mModelRenderResources[mObject.lock()->getID()] = asset->getMeshResource();
}

void MeshComponent::setMeshAsset(std::shared_ptr<ModelAsset> asset)
{
	mMeshAsset = asset;
}
