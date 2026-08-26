#include "Node.h"
#include <regex>
#include "Component.h"
size_t Node::getChildrenCount() const
{
    return children.size();
}

void Node::setParent(Node* parent)
{
    this->parent = parent;
    _transformDirty = true;
}

/// tag getter
int Node::getTag() const
{
    return _tag;
}

/// tag setter
void Node::setTag(int tag)
{
    _tag = tag;
}

const std::string& Node::getName() const
{
    return _name;
}

void Node::setName(const std::string& name)
{
    _name = name;
    std::hash<std::string> h;
    _hashOfName = h(name);
}


Node* Node::getChildByTag(int tag) const
{

    for (const auto child : children)
    {
        if (child && child->_tag == tag)
            return child;
    }
    return nullptr;
}

Node* Node::getChildByName(const std::string& name) const
{


    std::hash<std::string> h;
    size_t hash = h(name);

    for (const auto& child : children)
    {
        // Different strings may have the same hash code, but can use it to compare first for speed
        if (child->_hashOfName == hash && child->_name.compare(name) == 0)
            return child;
    }
    return nullptr;
}

void Node::enumerateChildren(const std::string& name, std::function<bool(Node*)> callback) const
{


    size_t length = name.length();

    size_t subStrStartPos = 0;  // sub string start index
    size_t subStrlength = length; // sub string length

    // Starts with '//'?
    bool searchRecursively = false;
    if (length > 2 && name[0] == '/' && name[1] == '/')
    {
        searchRecursively = true;
        subStrStartPos = 2;
        subStrlength -= 2;
    }

    // End with '/..'?
    bool searchFromParent = false;
    if (length > 3 &&
        name[length - 3] == '/' &&
        name[length - 2] == '.' &&
        name[length - 1] == '.')
    {
        searchFromParent = true;
        subStrlength -= 3;
    }

    // Remove '//', '/..' if exist
    std::string newName = name.substr(subStrStartPos, subStrlength);

    const Node* target = this;

    if (searchFromParent)
    {
        if (nullptr == parent)
        {
            return;
        }
        target = parent;
    }

    if (searchRecursively)
    {
        // name is '//xxx'
        target->doEnumerateRecursive(target, newName, callback);
    }
    else
    {
        // name is xxx
        target->doEnumerate(newName, callback);
    }
}
//递归查找某个节点
bool Node::doEnumerateRecursive(const Node* node, const std::string& name, std::function<bool(Node*)> callback) const
{
    bool ret = false;

    if (node->doEnumerate(name, callback))
    {
        // search itself
        ret = true;
    }
    else
    {
        // search its children
        for (const auto& child : node->getChildren())
        {
            if (doEnumerateRecursive(child, name, callback))
            {
                ret = true;
                break;
            }
        }
    }
   
    return ret;
}
//遍历直到找到目标node,支持多级,找不到就返回false
bool Node::doEnumerate(std::string name, std::function<bool(Node*)> callback) const
{
    // name may be xxx/yyy/zzz, should find its parent
    size_t pos = name.find('/');
    std::string searchName = name;//xxx
    bool needRecursive = false;
    if (pos != name.npos)
    {
        searchName = name.substr(0, pos);//xxx
        name.erase(0, pos + 1);//name = yyy/zzz
        needRecursive = true;
    }

    bool ret = false;
    for (const auto& child : children)
    {
        if (std::regex_match(child->_name, std::regex(searchName)))
        {
            if (!needRecursive)
            {
                // terminate enumeration if callback return true
                if (callback(child))
                {
                    ret = true;
                    break;
                }
            }
            else
            {
                ret = child->doEnumerate(name, callback);
                if (ret)
                    break;
            }
        }
    }

    return ret;
}



void Node::addChild(Node* child, int tag)
{
    addChildHelper(child, tag, "", true);
}

void Node::addChild(Node* child, int localZOrder, const std::string& name)
{
    addChildHelper(child, -1, name, false);
}
void Node::addChildHelper(Node* child, int tag, const std::string& name, bool setTag)
{
    auto assertNotSelfChild
    ([this, child]() -> bool
        {
            for (Node* parent(getParent()); parent != nullptr;
                parent = parent->getParent())
                if (parent == child)
                    return false;
            return true;
        });
    (void)assertNotSelfChild;

    if (!assertNotSelfChild())
        return;


    if (children.empty())
    {
        children.reserve(4);
    }

    this->insertChild(child);

    if (setTag)
        child->setTag(tag);
    else
        child->setName(name);

    child->setParent(this);


    if (enabled)
    {
        child->onEnter();

    }

}


void Node::addChild(Node* child)
{
    this->addChild(child, child->_name);
}

void Node::removeFromParent()
{
    this->removeFromParentAndCleanup(true);
}

void Node::removeFromParentAndCleanup(bool cleanup)
{
    if (parent != nullptr)
    {
        parent->removeChild(this, cleanup);
    }
}

/* "remove" logic MUST only be on this method
* If a class want's to extend the 'removeChild' behavior it only needs
* to override this method
*/
void Node::removeChild(Node* child, bool cleanup /* = true */)
{
    // explicit nil handling
    if (children.empty())
    {
        return;
    }

    ssize_t index = children.getIndex(child);
    if (index != CC_INVALID_INDEX)
        this->detachChild(child, index, cleanup);
}

void Node::removeChildByTag(int tag, bool cleanup/* = true */)
{
  

    Node* child = this->getChildByTag(tag);

    if (child == nullptr)
    {
    }
    else
    {
        this->removeChild(child, cleanup);
    }
}

void Node::removeChildByName(const std::string& name, bool cleanup)
{

    Node* child = this->getChildByName(name);

    if (child == nullptr)
    {
    }
    else
    {
        this->removeChild(child, cleanup);
    }
}

void Node::removeAllChildren()
{
    this->removeAllChildrenWithCleanup(true);
}

void Node::removeAllChildrenWithCleanup(bool cleanup)
{
    // not using detachChild improves speed here
    for (const auto& child : children)
    {
        // IMPORTANT:
        //  -1st do onExit
        //  -2nd cleanup
        if (enabled)
        {
            child->onExit();
        }

        if (cleanup)
        {
            child->cleanup();
        }

        // set parent nil at the end
        child->setParent(nullptr);
    }

    children.clear();
}

void Node::detachChild(Node* child, ssize_t childIndex, bool doCleanup)
{
    // IMPORTANT:
    //  -1st do onExit
    //  -2nd cleanup
    if (enabled)
    {
        child->onExit();
    }

    // If you don't do cleanup, the child's actions will not get removed and the
    // its scheduledSelectors_ dict will not get released!
    if (doCleanup)
    {
        child->cleanup();
    }


    // set parent nil at the end
    child->setParent(nullptr);

    children.erase(childIndex);
}
void Node::onEnter()
{


    if (_onEnterCallback)
        _onEnterCallback();

    if (compManager && !compManager->isEmpty())
    {
        compManager->onEnter();
    }


    for (const auto& child : children)
        child->onEnter();

    this->resume();

    enabled = true;


}
void Node::onExit()
{


    if (_onExitCallback)
        _onExitCallback();

    if (compManager && !compManager->isEmpty())
    {
        compManager->onExit();
    }

    this->pause();

    enabled = false;

    for (const auto& child : children)
        child->onExit();


}
void Node::resume()
{

}

void Node::pause()
{

}

// override me
void Node::update(float fDelta)
{

}
Component* Node::getComponent(const std::string& name)
{
    if (compManager)
        return compManager->get(name);

    return nullptr;
}

bool Node::addComponent(Component* component)
{
    // lazy alloc
    if (!compManager)
        compManager = new (std::nothrow) ComponentManager(this);

    // should enable schedule update, then all components can receive this call back
  

    return compManager->add(component);
}

bool Node::removeComponent(const std::string& name)
{
    if (compManager)
        return compManager->remove(name);

    return false;
}

bool Node::removeComponent(Component* component)
{
    if (compManager)
    {
        return compManager->remove(component);
    }

    return false;
}

void Node::removeAllComponents()
{
    if (compManager)
        compManager->removeAll();
}


glm::mat4 Node::getlocalMatrix()
{
    if (_transformDirty)
    {
        updateMatrix();
        _transformDirty = false;
    }
        
    return m_localMatrix;
}

glm::mat4 Node::getWorldMatrix()
{
    glm::mat4 mat = getlocalMatrix();

    for (Node* p = parent; p != nullptr; p = p->getParent())
    {
        mat = p->getlocalMatrix()*mat;
    }
    return mat;
}





void Node::updateMatrix()
{
    glm::mat4 matrix = glm::mat4(1.0f);
    matrix = glm::translate(matrix, m_position);
    glm::quat myQuat = glm::quat(glm::vec3(glm::radians(m_eulerAngle.x), 
        glm::radians(m_eulerAngle.y), glm::radians(m_eulerAngle.z)));
    // 将四元数转换为 4x4 旋转矩阵
    glm::mat4 rotationMatrix = glm::mat4_cast(myQuat);
    matrix = matrix * rotationMatrix;
    matrix = glm::scale(matrix, m_scale);
    this->m_localMatrix = matrix;

}
