#pragma once

#include "vulkan/vulkan.h"
#include "../utils/Utils.h"
#include "functional"
#define MAX_FRAMES_IN_FLIGHT 2

void DoTimes(std::function<void(int)>);
class Device
{
public:
	Device() = default;
	~Device() = default;
	Device(const Device&) = delete;
	Device& operator=(const Device&) = delete;
	static Device& getInstance() {
		static Device instance;
		return instance;
	}

public:
	VkDevice device = 0;
	VkInstance ins = 0;
	VkPhysicalDevice gpu = 0;
	uint32_t uniformAlign = 0;
	QueueFamilyIndices indices = {0};
	VkQueue graphicsQueue = 0;
	VkQueue presentQueue = 0;
	VkDebugUtilsMessengerEXT debugMessenger = 0;
	VkSurfaceKHR surface = 0;

	void init();
	void createDevice();
	void createInstance();
	void createSurface();
	void chooseGpu();
	void createDebugMesseger();
	void clean();
	void waitIdle();



};

