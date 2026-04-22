#include "VulkanEngine.h"
#include "audio/AudioEngine.h"
#include <iostream>
int main() {
    //VulkanEngine engine;

    try {
       // engine.Run();
       // AudioEngine::play2d("spring.mp3");
        int id =  AudioEngine::play2d("s2.mp3");
  
        getchar();
        AudioEngine::end();
    }
    catch (const std::exception& e) {
        std::cerr << e.what() << std::endl;
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}