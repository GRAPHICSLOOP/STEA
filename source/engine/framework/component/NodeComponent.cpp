#include "NodeComponent.h"

NodeComponent::NodeComponent()
{
}

NodeComponent::NodeComponent(std::shared_ptr<class Object> obj)
	:Component(obj)
{
}

void NodeComponent::addComponent(std::shared_ptr<Component> comp)
{
	mComponents.push_back(comp);
}

std::vector<std::shared_ptr<Component>> NodeComponent::getComponents()
{
	return mComponents;
}

