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

    DescriptorSetPool::clean();
    CommandBufferPool::clean();
    Allocator::clean();
    Device::getInstance().clean();
    glWindow::getInstance().clean();

    return EXIT_SUCCESS;
}