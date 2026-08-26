#pragma once
#include <unordered_map>
#include <string>
#include "Ref.h"
class Node;

class Component:public Ref
{
public:
    Component* create();
    virtual ~Component();

    virtual bool init();

    bool isEnabled() const { return _enabled; }
    virtual void setEnabled(bool enabled);

    const std::string& getName() const { return _name; }
    virtual void setName(const std::string& name) { _name = name; }

    Node* getOwner() const { return _owner; }
    virtual void setOwner(Node* owner);

    virtual void update(float delta);
    virtual bool serialize(void* r);

    virtual void onEnter();
    virtual void onExit();
    virtual void onAdd();
    virtual void onRemove();

public:

    Component();

protected:
    Node* _owner;
    std::string _name;
    bool _enabled;

};

class ComponentManager
{

protected:

    ComponentManager(Node* node);

public:

    ~ComponentManager();


    Component* get(const std::string& name) const;

    bool add(Component* com);
    bool remove(const std::string& name);
    bool remove(Component* com);
    void removeAll();
    void visit(float delta);

    void onEnter();
    void onExit();

    bool isEmpty() const { return _componentMap.empty(); }
private:
    std::unordered_map<std::string, Component*> _componentMap;
    Node* _owner;

    friend class Node;


	
};