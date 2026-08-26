#include "Component.h"
#include "Node.h"
Component* Component::create()
{
    Component* ret = new (std::nothrow) Component();

    if (ret && ret->init())
    {
        ret->autorelease();
    }
    else
    {
        if (ret)
            delete ret;
    }

    return ret;

}
Component::Component()
    : _owner(nullptr)
    , _enabled(true)
{

}

Component::~Component()
{
}

bool Component::init()
{
    return true;
}



void Component::onEnter()
{

}

void Component::onExit()
{

}

void Component::onAdd()
{

}

void Component::onRemove()
{

}

void Component::update(float /*delta*/)
{

}

bool Component::serialize(void* /*ar*/)
{
    return true;
}

void Component::setOwner(Node* owner)
{
    _owner = owner;
}

void Component::setEnabled(bool enabled)
{
    _enabled = enabled;
}


ComponentManager::ComponentManager(Node* node)
    : _owner(node)
{
}

ComponentManager::~ComponentManager()
{
}

Component* ComponentManager::get(const std::string& name) const
{
    Component* ret = nullptr;

    auto it = _componentMap.find(name);
    if (it != _componentMap.end())
    {
        ret = it->second;
    }

    return ret;
}

bool ComponentManager::add(Component* com)
{
    bool ret = false;

    do
    {
        auto componentName = com->getName();

        if (_componentMap.find(componentName) != _componentMap.end())
        {
            break;
        }
        _componentMap[componentName] = com;
        com->retain();
        com->setOwner(_owner);
        com->onAdd();

        ret = true;
    } while (0);
    return ret;
}

bool ComponentManager::remove(const std::string& componentName)
{
    bool ret = false;
    do
    {
        auto iter = _componentMap.find(componentName);
        if (iter == _componentMap.end()) break;

        auto component = iter->second;
        _componentMap.erase(componentName);

        component->onRemove();
        component->setOwner(nullptr);
        component->release();

        ret = true;
    } while (0);

    return ret;
}

bool ComponentManager::remove(Component* com)
{
    return remove(com->getName());
}

void ComponentManager::removeAll()
{
    if (!_componentMap.empty())
    {
        for (auto& iter : _componentMap)
        {
            iter.second->onRemove();
            iter.second->setOwner(nullptr);
            iter.second->release();
        }

        _componentMap.clear();
       // _owner->unscheduleUpdate();
    }
}

void ComponentManager::visit(float delta)
{
    if (!_componentMap.empty())
    {
        SAFE_RETAIN(_owner);
        for (auto& iter : _componentMap)
        {
            iter.second->update(delta);
        }
        SAFE_RELEASE(_owner);
    }
}

void ComponentManager::onEnter()
{
    for (auto& iter : _componentMap)
    {
        iter.second->onEnter();
    }
}

void ComponentManager::onExit()
{
    for (auto& iter : _componentMap)
    {
        iter.second->onExit();
    }
}