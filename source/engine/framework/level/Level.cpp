#include "Level.h"
#include "../component/MeshComponent.h"
#include "../component/TransformComponent.h"
#include "core/base/macro.h"

void Level::initialize()
{
	uint32_t numb = 4;
	for (uint32_t i = 0; i < numb; i++)
	{
		for (uint32_t j = 0; j < numb; j++)
		{
			std::shared_ptr<Object> newObject = createObject();
			std::shared_ptr<MeshComponent> meshComponent = std::make_shared<MeshComponent>(newObject);
			std::shared_ptr<TransformComponent> transformComponent = std::make_shared<TransformComponent>(newObject);
			meshComponent->attachToComponent(newObject->mRootComponent);
			transformComponent->attachToComponent(newObject->mRootComponent);
			newObject->showMeshTemp();
			transformComponent->setScale(glm::vec3(0.2f));
			transformComponent->setPosistion(glm::vec3(i * 1.5f, 0, j * 1.5f));
		}
	}

}

void Level::tick(float delta)
{
	for (const auto obj : mObjects)
	{
		obj->tick(delta);
	}
}

std::shared_ptr<Object> Level::createObject()
{
	std::shared_ptr<Object> newObject = std::make_shared<Object>(ObjectIDAllocator::Alloc());
	addObject(newObject);
	return newObject;
}

void Level::addObject(std::shared_ptr<Object> object)
{
	mObjects.push_back(object);
}

void Level::removeObject(std::shared_ptr<Object> object)
{
	mObjects.emplace_back(object);
	mObjects.pop_back();
}

std::vector<std::shared_ptr<Object>> Level::getObjects()
{
	return mObjects;
}
