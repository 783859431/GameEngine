#include "glWindow.h"
#include "../input/Input.h"


void glWindow::FramebufferResizeCallback(GLFWwindow* window, int width, int height)
{
    auto win = reinterpret_cast<glWindow*>(glfwGetWindowUserPointer(window));
    win->framebufferResized = true;
    win->width = width;
    win->height = height;
}

void glWindow::createWindow(int width,int height)
{
    this->width = width;
    this->height = height;
    glfwInit();
    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
    glfwWindowHint(GLFW_RESIZABLE, GLFW_TRUE);
    window = glfwCreateWindow(width, height, "MainView", nullptr, nullptr);//创建窗口
    glfwSetWindowUserPointer(window, this);//传递用户数据给窗口函数
    glfwSetFramebufferSizeCallback(window, FramebufferResizeCallback);//重建窗口大小
    glfwSetScrollCallback(window, Input::Mouse::Mouse_ScrollCallback);//获取鼠标滚轮
}

glm::vec2 glWindow::getFrameSize()
{
    int w, h;
    glfwGetFramebufferSize(window, &w, &h);
    return glm::vec2(w,h);
}

void glWindow::clean()
{
    glfwDestroyWindow(window);
    glfwTerminate();
   

}

glWindow::glWindow() {

}