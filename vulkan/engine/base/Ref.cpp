#include "Ref.h"
#include "AutoReleasePool.h"
Ref::Ref()
    : refCount(1) // when the Ref is created, the reference count of it is 1

{

}

Ref::~Ref()
{

}

void Ref::retain()
{

	++refCount;
}

void Ref::release()
{
	--refCount;
	if (refCount == 0)
		delete this;
}

Ref* Ref::autorelease()
{
	ReleasePoolManager::getInstance()->getCurrentPool()->add(this);
	return this;
}
