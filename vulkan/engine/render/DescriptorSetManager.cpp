#include "DescriptorSetManager.h"
#include "Device.h"
// 先在的问题是每个物体需要独立的一块内存来存放uniform 矩阵，如果直接在同一块内存上放uniform矩阵，
// 无法做到gpu完成第一个物体的绘制后更新uniform矩阵再绘制新的物体。




void DPool::allocSets(std::vector<DSetLayout*>& setLayouts, std::vector<VkDescriptorSet>& outSets)
{

    if (outSets.size() < setLayouts.size())
        outSets.resize(setLayouts.size());

    _allocSets(setLayouts.data(),setLayouts.size(),outSets.data());


}
void DPool::_allocSets(DSetLayout** setLayouts, uint32_t count, VkDescriptorSet* sets)
{
    std::vector<VkDescriptorSetLayout> temp;
    for (int i = 0; i < count; i++)
    {
        temp.push_back(setLayouts[i]->layout);
    }
    VkDescriptorSetAllocateInfo allocInfo{};
    allocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
    allocInfo.descriptorPool = this->setsPool;
    allocInfo.pSetLayouts = temp.data();
    allocInfo.descriptorSetCount = count;
    if (vkAllocateDescriptorSets(Device::getInstance().device, &allocInfo, sets) != VK_SUCCESS) {
        throw std::runtime_error("failed to create set");
    }
    this->setUsage.usage += count;
    this->setUsage.remain -= count;

    for (int i=0;i<count;i++)
    {
        for (auto [k, v] : setLayouts[i]->layoutInfo)
        {
            this->poolInfo[k].usage += v;
            this->poolInfo[k].remain -= v;
        }
    }
}

void DPool::clean()
{
    if(this->setsPool)
        vkDestroyDescriptorPool(Device::getInstance().device,this->setsPool,0);
}


void PoolManager::init()
{

    std::vector<VkDescriptorPoolSize> poolSize = { 
        {VK_DESCRIPTOR_TYPE_SAMPLER ,100},
        {VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER,100},
        {VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,100},
        {VK_DESCRIPTOR_TYPE_STORAGE_BUFFER,100},
        {VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC,10} };

    createPool(poolSize,100);

}

void PoolManager::allocSets(std::vector<DSetLayout*>& layouts, std::vector<VkDescriptorSet>& outSets)
{
    int poolIndex = findOrCreatePool(layouts);
    pools[poolIndex]->allocSets(layouts,outSets);
}

void PoolManager::allocSets(DSetLayout** layouts, int count, VkDescriptorSet* outsets)
{
    std::vector<DSetLayout*> temp;
    for (int i = 0; i < count; i++)
    {
        temp.push_back(layouts[i]);

    }
    int poolIndex = findOrCreatePool(temp);
    pools[poolIndex]->_allocSets(layouts, count, outsets);
   
}


void PoolManager::allocSet(DSetLayout& layout, VkDescriptorSet* outset)
{
    std::vector<DSetLayout*> temp;
    temp.push_back(&layout);
    int poolIndex = findOrCreatePool(temp);
    pools[poolIndex]->_allocSets(temp.data(), 1, outset);
}

int  PoolManager::createPool(std::vector<VkDescriptorPoolSize>& poolSize,uint32_t maxSets)
{

    VkDescriptorPoolCreateInfo descriptorPoolInfo{};
    descriptorPoolInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
    descriptorPoolInfo.poolSizeCount = static_cast<uint32_t>(poolSize.size());
    descriptorPoolInfo.pPoolSizes = poolSize.data();
    descriptorPoolInfo.maxSets = maxSets;
    descriptorPoolInfo.flags = VK_DESCRIPTOR_POOL_CREATE_FREE_DESCRIPTOR_SET_BIT;
    VkDescriptorPool p;
    if (vkCreateDescriptorPool(Device::getInstance().device, &descriptorPoolInfo, nullptr, &p) !=
        VK_SUCCESS) {
        throw std::runtime_error("failed to create descriptor pool!");
    }

    auto pool = std::make_unique<DPool>();
 
    for (int i = 0; i < poolSize.size(); i++)
    {
        pool->poolInfo[poolSize[i].type] = { poolSize[i].descriptorCount,0,poolSize[i].descriptorCount };

    }
    pool->setUsage = { maxSets,0,maxSets };
    pool->setsPool = p;
    pools.push_back(std::move(pool));

    return pools.size()-1;
}

int PoolManager::findOrCreatePool(std::vector<DSetLayout*>& layouts)
{


    std::unordered_map<VkDescriptorType, uint32_t> total;
    //统计所有资源类型和数量
    for (int i = 0; i < layouts.size(); i++)
    {
        auto& info = layouts[i]->layoutInfo;
        for (auto o : info)
        {
            total[o.first] += o.second;
        }
    }
    //判断当前的pool能否满足所需资源
    int find = -1;
    for (int i = 0; i < pools.size(); i++)
    {
        bool isOk = true;
        for (auto [tp, count] : total)
        {
            auto it = this->pools[i]->poolInfo.find(tp);
            bool has = (it != this->pools[i]->poolInfo.end());//当前描述符类型是否存在pool中
            //当前描述符剩余数量是否足够
            //当前pool剩余的set数量是否足够
            if (!has || (it->second.remain < count)|| pools[i]->setUsage.remain<layouts.size())
            {
                isOk = false;
                break;
            }
        }
        if (isOk)
        {
            find = i;
            break;
        }

    }

    //

    if (find==-1)
    {
        std::vector<VkDescriptorPoolSize> poolSize;
        uint32_t max = 0;
        for (auto [tp, count] : total)
        {
            max = count > max ? count : max;
            poolSize.push_back({ tp,count * 2 });
        }
       find = createPool(poolSize,max*2);

    }

    return find;
}

void PoolManager::clean()
{

    for (auto& p : pools)
    {
        p->clean();
    }
}
