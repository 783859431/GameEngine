#pragma once
#include "vulkan/vulkan.h"
#include <string>
#include <vector>
#include <fstream>
#include "Global.h"
#include "Device.h"
class Shader
{

public:

    static char* readFile(const std::string& filename, size_t* s);
    static VkShaderModule LoadShader(std::string path);

};

