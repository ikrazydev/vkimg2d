#pragma once

#define GLFW_INCLUDE_NONE
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include <vulkan/vulkan.h>

static const uint32_t WIN_WIDTH = 800;
static const uint32_t WIN_HEIGHT = 600;

struct WindowConfig
{
};

class Window
{
public:
    Window(WindowConfig config);
    ~Window();

    void create();

    void vkGetRequiredExtensions(const char**& exts, uint32_t& extCount) const;
    VkResult vkCreateSurface(VkInstance instance, const VkAllocationCallbacks* allocator, VkSurfaceKHR* surface) const;

    void getWindowSize(uint32_t* width, uint32_t* height) const;
    void getFramebufferSize(uint32_t* width, uint32_t* height) const;
    void getPointScaling(float* scale) const;
    void getScaling(float* xScale, float* yScale) const;

    bool shouldClose() const;
    void update();

    GLFWwindow* getGLFWHandle() const;
private:
    static void _framebufferResizeCallback(GLFWwindow* window, int width, int height);

    GLFWwindow* mWindow;
    bool mFramebufferResized = false;
};
