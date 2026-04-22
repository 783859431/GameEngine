#pragma once
#include "vulkan/vulkan.h"
#include "glm/glm.hpp"
#include <iostream>
#include "Global.h"
#include <fstream>
#include <vector>
#include <set>
#include <optional>
#ifdef NDEBUG
const bool enableValidationLayers = false;
#else
const bool enableValidationLayers = true;
#endif


struct QueueFamilyIndices {
    std::optional<uint32_t> graphicsFamily;
    std::optional<uint32_t> presentFamily;
    bool isComplete() {
        return graphicsFamily.has_value() && presentFamily.has_value();
    }
};
namespace Utils
{

    void vk_check(VkResult res);
    std::vector<const char*> getRequiredExtensions();
    QueueFamilyIndices findQueueFamilies(VkPhysicalDevice device, VkSurfaceKHR surface);
    std::vector<char> readFile(const std::string& filename);



};

