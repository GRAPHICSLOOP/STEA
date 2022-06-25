#pragma once
#include "../component/NodeComponent.h"
#include "../inputSystem/InputSystem.h"
#include "ObjectIDAllocator.h"

class Object : public std::enable_shared_from_this<Object>
{
public:
	Object(ObjectID id);
	~Object();
	void tick(float delta);
	void showMeshTemp();

	size_t getID();

public:
	std::shared_ptr<NodeComponent> mRootComponent;

private:
	ObjectID mID;
};



