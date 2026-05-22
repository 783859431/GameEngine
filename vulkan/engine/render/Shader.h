#pragma once
#include "vulkan/vulkan.h"
#include <string>
#include <vector>
#include <fstream>
#include "Device.h"
class Shader
{

public:

    static VkShaderModule LoadShader(std::string path);

};

