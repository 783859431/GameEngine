#include "Shader.h"

char* Shader::readFile(const std::string& filename, size_t* s)
{
    std::ifstream file(filename, std::ios::ate | std::ios::binary);

    if (!file.is_open()) {
        throw std::runtime_error("failed to open file!");
    }

    size_t fileSize = (size_t)file.tellg();
    char* buffer = new char[fileSize];

    file.seekg(0);
    file.read(buffer, fileSize);

    file.close();
    *s = fileSize;
    return buffer;
}



VkShaderModule Shader::LoadShader(std::string path)
{
    VkShaderModule mod = VK_NULL_HANDLE;
    size_t s = 0;
    char* shader = readFile(path, &s);
    VkShaderModuleCreateInfo createInfo{};
    createInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
    createInfo.codeSize = s;
    createInfo.pCode = reinterpret_cast<const uint32_t*>(shader);

    VkShaderModule shaderModule;
    if (vkCreateShaderModule(Device::Instance().device, &createInfo, nullptr, &shaderModule) != VK_SUCCESS) {
        throw std::runtime_error("failed to create shader module!");
    }
    delete[] shader;

    return shaderModule;
}
