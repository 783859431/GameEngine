#pragma once
#include "vulkan/vulkan.h"
#include "vk_mem_alloc.h"
#include "Allocator.h"
#include <memory>

class VBuffer
{ 
protected:
   AllocatedBuffer bf;
public:
    VBuffer() {};

    VBuffer& operator=(VBuffer&) = delete;

    VBuffer(const VBuffer&) = delete;

    VBuffer(VBuffer&& other) noexcept
    {
        this->bf = other.bf;
        other.bf._allocation = 0;
        
    }
    virtual void alloc(uint32_t size)=0;
    
    virtual void copy(void* data, uint32_t size, uint32_t offset = 0);

    void clean();

    VkBuffer getBuffer()
    {
        return bf._buffer;
    }

    virtual ~VBuffer()
    {
        clean();
    }
};

