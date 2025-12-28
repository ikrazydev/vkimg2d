#pragma once

#include <optional>

#include <app_data.hpp>
#include <window.hpp>
#include <vulkan/renderer.hpp>

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

    AppData mAppData;

    Window mWindow;
    std::optional<VkRenderer> mVkRenderer;
};
