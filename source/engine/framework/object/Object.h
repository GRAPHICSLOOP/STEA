#pragma once
#include "../component/NodeComponent.h"
#include "../inputSystem/InputSystem.h"
#include "ObjectIDAllocator.h"

class Object : public InputSystem , public std::enable_shared_from_this<Object>
{
public:
	Object(ObjectID id);
	~Object();
	void tick(float delta);

	size_t getID();

protected:
	void onKey(int key, int scancode, int action, int mods);

public:
	std::shared_ptr<NodeComponent> mRootComponent;

private:
	ObjectID mID;
};



