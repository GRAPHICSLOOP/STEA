#include "ObjectIDAllocator.h"
#include <limits>
#include "core/base/macro.h"

std::atomic<ObjectID> ObjectIDAllocator::mNextID = 0;

ObjectID ObjectIDAllocator::Alloc()
{
    std::atomic<ObjectID> newObjectRet = mNextID.load();
    mNextID++;
    if (mNextID >= std::numeric_limits<std::size_t>::max())
    {
        STEALOG_ERROR("object id overflow");
    }

    return newObjectRet;
}
