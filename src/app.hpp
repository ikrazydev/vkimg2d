#pragma once

#include <vulkan/vulkan.hpp>

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

const uint32_t WIDTH = 800;
const uint32_t HEIGHT = 600;

class VkImg2DApp {
public:
    void run();

    void initWindow();

    void initVulkan();
    void createInstance();

    void mainLoop();

    void cleanup();
private:
    GLFWwindow* mWindow;

    VkInstance mInstance;
};
