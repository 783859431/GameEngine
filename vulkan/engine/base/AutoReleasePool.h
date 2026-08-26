#pragma once
#include <string>
#include <vector>
class Ref;

class AutoReleasePool
{
public:
    AutoReleasePool();
    AutoReleasePool(const std::string& name);
    void add(Ref* obj);
    void clear();
    bool contains(Ref* object) const;
    std::vector<Ref*> objs;
    std::string _name;
};

class ReleasePoolManager
{

public:
    static ReleasePoolManager* getInstance();
    static void destroyInstance();


    AutoReleasePool* getCurrentPool() const;

    bool isObjectInPools(Ref* obj) const;
    friend class AutoreleasePool;


    ReleasePoolManager();
    ~ReleasePoolManager();

    void push(AutoReleasePool* pool);
    void pop();

    static ReleasePoolManager* s_singleInstance;

    std::vector<AutoReleasePool*> _releasePoolStack;
};