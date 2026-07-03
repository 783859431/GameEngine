#include "engine/Engine.h"
#include <iostream>
int main() {
 

    try {
       VulkanEngine engine;
       engine.Run();

    }
    catch (const std::exception& e) {
        std::cerr << e.what() << std::endl;
    }

    PoolManager::inst().clean();
    CommandBufferPool::clean();
    Allocator::clean();
    SamplerManager::clean();
    Device::getInstance().clean();
    glWindow::getInstance().clean();
    
    return EXIT_SUCCESS;
}