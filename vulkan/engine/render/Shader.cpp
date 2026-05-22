#include "Shader.h"
#include "../utils/FileUtils.h"


VkShaderModule Shader::LoadShader(std::string path)
{
    VkShaderModule mod = VK_NULL_HANDLE;
    std::vector<char> buf;
    FileUtils::GetInst()->Read(path,buf);
    VkShaderModuleCreateInfo createInfo{};
    createInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
    createInfo.codeSize = buf.size();
    createInfo.pCode = reinterpret_cast<const uint32_t*>(buf.data());
    VkShaderModule shaderModule;
    if (vkCreateShaderModule(Device::getInstance().device, &createInfo, nullptr, &shaderModule) != VK_SUCCESS) {
        throw std::runtime_error("failed to create shader module!");
    }

    return shaderModule;
}

void CompileShader(std::string path, std::string outPath)
{
	std::string cmd = "glslc " + path + " -o " + outPath;
	system(cmd.c_str());
}
