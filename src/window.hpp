#pragma once

#define GLFW_INCLUDE_NONE
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

static const uint32_t WIN_WIDTH = 800;
static const uint32_t WIN_HEIGHT = 600;

class Window
{
public:
    void init();

    bool shouldClose();
    void update();
private:
    static void _framebufferResizeCallback(GLFWwindow* window, int width, int height);

    GLFWwindow* mWindow;
    bool mFramebufferResized = false;
};
