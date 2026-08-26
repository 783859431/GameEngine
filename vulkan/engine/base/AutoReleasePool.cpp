#include "AutoReleasePool.h"
#include "Ref.h"
AutoReleasePool::AutoReleasePool()
    : _name("")

{
    objs.reserve(150);
    ReleasePoolManager::getInstance()->push(this);
}

AutoReleasePool::AutoReleasePool(const std::string& name)
    : _name(name)

{
    objs.reserve(150);
    ReleasePoolManager::getInstance()->push(this);
}

AutoReleasePool::~AutoReleasePool()
{
    clear();
    ReleasePoolManager::getInstance()->pop();
}


void AutoReleasePool::add(Ref* obj)
{
    objs.push_back(obj);
}

void AutoReleasePool::clear()
{
    std::vector<Ref*> releasings;
    releasings.swap(objs);
    for (const auto& obj : releasings)
    {
        obj->release();
    }

}

bool AutoReleasePool::contains(Ref* object) const
{
    for (const auto& obj : objs)
    {
        if (obj == object)
            return true;
    }
    return false;
}

ReleasePoolManager* ReleasePoolManager::s_singleInstance = nullptr;
ReleasePoolManager::ReleasePoolManager() 
{
    _releasePoolStack.reserve(10);
}
ReleasePoolManager* ReleasePoolManager::getInstance()
{
    if (s_singleInstance == nullptr)
    {
        s_singleInstance = new (std::nothrow) ReleasePoolManager();
        // Add the first auto release pool
        new AutoReleasePool("poo0");
    }
    return s_singleInstance;
}

void ReleasePoolManager::destroyInstance()
{
    delete s_singleInstance;
    s_singleInstance = nullptr;

}

AutoReleasePool* ReleasePoolManager::getCurrentPool() const
{
    return _releasePoolStack.back();
}

bool ReleasePoolManager::isObjectInPools(Ref* obj) const
{
    for (const auto& pool : _releasePoolStack)
    {
        if (pool->contains(obj))
            return true;
    }
    return false;
}

ReleasePoolManager::~ReleasePoolManager()
{
    while (!_releasePoolStack.empty())
    {
        AutoReleasePool* pool = _releasePoolStack.back();

        delete pool;
    }
}

void ReleasePoolManager::push(AutoReleasePool* pool)
{
    _releasePoolStack.push_back(pool);

}

void ReleasePoolManager::pop()
{
    _releasePoolStack.pop_back();
}
