#include <window.hpp>

Window::~Window()
{
    glfwDestroyWindow(mWindow);
    glfwTerminate();
}

void Window::init()
{
    glfwInit();

    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);

    mWindow = glfwCreateWindow(WIN_WIDTH, WIN_HEIGHT, "VkImg2D", nullptr, nullptr);
    glfwSetWindowUserPointer(mWindow, this);
    glfwSetFramebufferSizeCallback(mWindow, _framebufferResizeCallback);
}

void Window::vkGetRequiredExtensions(const char**& exts, uint32_t& extCount)
{
    exts = glfwGetRequiredInstanceExtensions(&extCount);
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
