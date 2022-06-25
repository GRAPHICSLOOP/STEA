#include "Object.h"
#include "core/base/macro.h"
#include "../component/MeshComponent.h"
#include "../component/TransformComponent.h"

Object::Object(ObjectID id)
{
	mID = id;
	mRootComponent = std::make_shared<NodeComponent>();
}

Object::~Object()
{
	
}

void Object::tick(float delta)
{
	auto comps = mRootComponent->getComponents();
	for (auto& com : comps)
	{
		com->tick(delta);
	}

}

void Object::showMeshTemp()
{
	MeshComponent* meshComponent = mRootComponent->getComponent<MeshComponent>();
	if (meshComponent)
	{
		//meshComponent->TempLoadMeshAsset("TestNewMesh", "models/viking_room.obj");
		meshComponent->TempLoadMeshAsset("TestNewMesh", "models/nanosuit/nanosuit.obj");
	}
	else
	{
		STEALOG_ERROR("meshComponent NULL");
	}

	TransformComponent* transformComponent = mRootComponent->getComponent<TransformComponent>();
	if (transformComponent)
	{
		transformComponent->setScale(glm::vec3(0.1f));
	}
}

size_t Object::getID()
{
	return mID;
}
