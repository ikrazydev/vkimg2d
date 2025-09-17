#pragma once

#include <window.hpp>
#include <vulkan/renderer.hpp>

class App
{
public:
    void run();
private:
    void _initWindow();

    std::vector<const char*> _getExtensionNames();
    void _initVulkan();

    void _loop();

    Window mWindow;
    VkRenderer mVkRenderer;
};
