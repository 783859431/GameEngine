#include "MyGame.h"
#include <iostream>
int main() {
 

    try 
    {
        VulkanEngine* engine = new MyGame();
        engine->Init();
        engine->Run();
        engine->CleanUp();
        delete engine;

    }
    catch (const std::exception& e) 
    {
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