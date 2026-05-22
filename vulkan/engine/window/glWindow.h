#pragma once
#include "vulkan/vulkan.h"
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <string>
class glWindow
{   
	glWindow();
	GLFWwindow* window;
	bool framebufferResized = false;
	std::string windowName;
	
public:
	int width;
	int height;
	static glWindow& getInstance() {
		static glWindow w;
		return w;
	}
	static void FramebufferResizeCallback(GLFWwindow* window, int width, int height);
	void createWindow(int w,int h);
	glm::vec2  getFrameSize();
	void clean();
	glWindow(const glWindow&) = delete;
	glWindow& operator=(const glWindow&) = delete;
	
	bool shouldClose() { return glfwWindowShouldClose(window); }
	VkExtent2D getExtent() { return { static_cast<uint32_t>(width), static_cast<uint32_t>(height) }; }
	bool wasWindowResized() { return framebufferResized; }
	void resetWindowResizedFlag() { framebufferResized = false; }
	GLFWwindow* getGLFWwindow() const { return window; }

};

