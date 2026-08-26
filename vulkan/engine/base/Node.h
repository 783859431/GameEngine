#pragma once
#include <vector>
#include <string>
#include <functional>
#include "Ref.h"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/quaternion.hpp>
class Component;
class ComponentManager;


/// <summary>
/// 我对Node 的理解:
/// 首先，node组成了多叉树，你可以遍历和查询这个多叉树,删除和插入子节点。
/// Node的父子关系会直接影响物体的坐标，子物体的transform是基于父物体的，为一个localtransform。
/// 如果子物体没有父物体那么，子物体的transform就是localtransform =  worldtransform ;

/// </summary>
class Node:public Ref
{



    virtual void enumerateChildren(const std::string& name, std::function<bool(Node* node)> callback) const;
    virtual std::vector<Node*>& getChildren() { return children; }
    virtual const std::vector<Node*>& getChildren() const { return children; }
    virtual size_t getChildrenCount() const;
    virtual void setParent(Node* parent);
    int getTag() const;
    void setTag(int tag);
    const std::string& getName() const;
    void setName(const std::string& name);
    Node* getChildByTag(int tag) const;
    Node* getChildByName(const std::string& name) const;
    virtual  Node* getParent() const { return parent; }
    void addChild(Node* child);
    virtual void removeFromParent();
    virtual void removeFromParentAndCleanup(bool cleanup);
    virtual void removeChild(Node* child, bool cleanup = true);
    virtual void removeChildByTag(int tag, bool cleanup = true);
    virtual void removeChildByName(const std::string& name, bool cleanup = true);
    virtual void removeAllChildren();
    void removeAllChildrenWithCleanup(bool cleanup);
    bool doEnumerate(std::string name, std::function<bool(Node*)> callback) const;
    void addChild(Node* child, int localZOrder, int tag);
    bool doEnumerateRecursive(const Node* node, const std::string& name, std::function<bool(Node*)> callback) const;
    void enumerateChildren(const std::string& name, std::function<bool(Node*)> callback) const;
    void addChild(Node* child, int localZOrder, const std::string& name);
    void addChildHelper(Node* child, int tag, const std::string& name, bool setTag);
    void addChild(Node* child, int zOrder);
    Component* getComponent(const std::string& name);
    virtual bool addComponent(Component* component);
    virtual bool removeComponent(const std::string& name);
    virtual bool removeComponent(Component* component);
    virtual void removeAllComponents();

    void detachChild(Node* child, size_t childIndex, bool doCleanup);

    virtual void onEnter();
    virtual void update(float delta);
    virtual void onExit();
    virtual void resume();
    virtual void pause();

	std::vector<Node*> children;
	Node* parent;
	std::string _name; 
    int _tag;
	size_t _hashOfName;             
    ComponentManager* compManager;
    std::function<void()> _onEnterCallback;
    std::function<void()> _onExitCallback;
    bool _transformDirty;
    bool enabled;
    //
    glm::vec3 m_position = glm::vec3(0.0f);
    glm::vec3 m_eulerAngle = glm::vec3(0.0f); // 弧度
    glm::vec3 m_scale = glm::vec3(1.0f);      // V
    glm::mat4 m_localMatrix = glm::mat4(1.0f);

    glm::mat4 getlocalMatrix();
    glm::mat4 getWorldMatrix();
    void updateMatrix();


};

