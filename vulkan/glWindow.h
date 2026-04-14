#pragma once
#include "vulkan/vulkan.h"
#include "Input.h"
#include "Utils.h"
#include "singleton.h"
class glWindow:public Singleton<glWindow>
{   
	glWindow();
	friend class Singleton<glWindow>;
	GLFWwindow* window;
	bool framebufferResized = false;
	std::string windowName;

public:
	int width;
	int height;
	void init();
	static void FramebufferResizeCallback(GLFWwindow* window, int width, int height);
	void CreateWindow();
	glm::vec2  GetFrameSize();
	void Clean();
	glWindow(const glWindow&) = delete;
	glWindow& operator=(const glWindow&) = delete;
	
	bool shouldClose() { return glfwWindowShouldClose(window); }
	VkExtent2D getExtent() { return { static_cast<uint32_t>(width), static_cast<uint32_t>(height) }; }
	bool wasWindowResized() { return framebufferResized; }
	void resetWindowResizedFlag() { framebufferResized = false; }
	GLFWwindow* getGLFWwindow() const { return window; }

};

