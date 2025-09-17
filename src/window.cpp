#include <window.hpp>

void Window::init() {
    glfwInit();

    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);

    mWindow = glfwCreateWindow(WIN_WIDTH, WIN_HEIGHT, "VkImg2D", nullptr, nullptr);
    glfwSetWindowUserPointer(mWindow, this);
    glfwSetFramebufferSizeCallback(mWindow, _framebufferResizeCallback);
}

bool Window::shouldClose()
{
    return glfwWindowShouldClose(mWindow);
}

void Window::update()
{
    glfwPollEvents();
}

void Window::_framebufferResizeCallback(GLFWwindow* window, int, int) 
{
    auto win = reinterpret_cast<Window*>(glfwGetWindowUserPointer(window));
    win->mFramebufferResized = true;
}
