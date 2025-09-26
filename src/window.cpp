#include <window.hpp>

#include <stdexcept>

Window::Window(WindowConfig config) : mWindow(nullptr)
{
    glfwInit();

    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
}

Window::~Window()
{
    glfwDestroyWindow(mWindow);
    glfwTerminate();
}

void Window::create()
{
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

void Window::getFramebufferSize(uint32_t* width, uint32_t* height) const
{
    int w, h;
    glfwGetFramebufferSize(mWindow, &w, &h);

    *width = static_cast<uint32_t>(w);
    *height = static_cast<uint32_t>(h);
}

bool Window::shouldClose() const
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
