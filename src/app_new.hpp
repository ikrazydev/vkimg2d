#pragma once

#include <window.hpp>
#include <vulkan/renderer.hpp>

#include <optional>

class App
{
public:
    App();

    void run();
private:
    void _createWindow();

    std::vector<const char*> _getExtensionNames() const;
    void _initVulkan();

    void _loop();

    Window mWindow;
    std::optional<VkRenderer> mVkRenderer;
};
