#include <window.hpp>

#include <stdexcept>

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

void Window::vkGetRequiredExtensions(const char**& exts, uint32_t& extCount) const
{
    exts = glfwGetRequiredInstanceExtensions(&extCount);
}

VkResult Window::vkCreateSurface(VkInstance instance, const VkAllocationCallbacks* allocator, VkSurfaceKHR* surface) const
{
    return glfwCreateWindowSurface(instance, mWindow, allocator, surface);
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
