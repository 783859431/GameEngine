#pragma once
#include "vulkan/vulkan.h"
#include "singleton.h"
#include "Utils.h"
class Device:public Singleton<Device>
{
	friend class Singleton<Device>;
public:
	VkDevice device;
	VkInstance ins;
	VkPhysicalDevice gpu;
	QueueFamilyIndices indices;
	VkQueue graphicsQueue;
	VkQueue presentQueue;
	VkDebugUtilsMessengerEXT debugMessenger;
	VkSurfaceKHR surface;

	void init();
	void createDevice();
	void createInstance();
	void createSurface();
	void chooseGpu();
	void createDebugMesseger();
	void Clean();
	void waitIdle();



};

