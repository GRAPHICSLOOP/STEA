#include "Component.h"
#include "NodeComponent.h"
#include "../object/Object.h"

Component::Component()
{
}

Component::Component(std::shared_ptr<Object> obj)
{
	mObject = obj;
}

Component::~Component()
{
}

void Component::attachToComponent(std::weak_ptr<NodeComponent> component)
{
	mParentComponent = component;
	if (mParentComponent.lock())
	{
		mParentComponent.lock()->addComponent(shared_from_this());
	}
}

void Component::tick(float delta)
{

}
