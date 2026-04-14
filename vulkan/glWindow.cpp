#include "glWindow.h"
#include "Global.h"
//创建基础窗口
void glWindow::init()
{
    CreateWindow();
}

void glWindow::FramebufferResizeCallback(GLFWwindow* window, int width, int height)
{
    auto win = reinterpret_cast<glWindow*>(glfwGetWindowUserPointer(window));
    win->framebufferResized = true;
    win->width = width;
    win->height = height;
}
void glWindow::CreateWindow()
{
    glfwInit();
    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
    glfwWindowHint(GLFW_RESIZABLE, GLFW_TRUE);
    window = glfwCreateWindow(width, height, "Vulkan", nullptr, nullptr);//创建窗口
    glfwSetWindowUserPointer(window, this);//传递用户数据给窗口函数
    glfwSetFramebufferSizeCallback(window, FramebufferResizeCallback);//重建窗口大小
    glfwSetScrollCallback(window, Input::Mouse::Mouse_ScrollCallback);//获取鼠标滚轮
}



glm::vec2 glWindow::GetFrameSize()
{
    int w, h;
    glfwGetFramebufferSize(window, &w, &h);
    return glm::vec2(w,h);
}


void glWindow::Clean()
{
    glfwDestroyWindow(window);
    glfwTerminate();
   

}
glWindow::glWindow() {
    width = WINDOW_WIDTH;
    height = WINDOW_HEIGHT;

}