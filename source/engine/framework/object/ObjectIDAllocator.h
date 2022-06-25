#pragma once
#include <atomic>


using ObjectID = std::size_t;

class ObjectIDAllocator
{
public:
	static ObjectID Alloc();

private:
	static std::atomic<ObjectID> mNextID;
};


