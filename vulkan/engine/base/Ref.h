#pragma once
#define SAFE_RETAIN(ref) do{if(ref) ref->retain();}while(0);
#define SAFE_RELEASE(ref) do{if(ref) ref->release();}while(0);
//内存管理
class Ref
{
public:

    Ref();

    ~Ref();

    void retain();

    void release();

    Ref* autorelease();

	unsigned int refCount = 0;

    friend class AutoreleasePool;
};

