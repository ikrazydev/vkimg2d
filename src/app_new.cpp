#include "app_new.hpp"

#if DEBUG
    static const bool gEnableValidationLayers  = true;
#else
    static const bool gEnableValidationLayers  = false;
#endif

static const std::vector<const char*> gDeviceExtensions = {
#ifdef __APPLE__
    "VK_KHR_portability_subset",
#endif

    VK_KHR_SWAPCHAIN_EXTENSION_NAME,
};

static const std::vector<const char*> gValidationLayers = {
    "VK_LAYER_KHRONOS_validation",
};

void App::run()
{
    _initWindow();
    _initVulkan();

    _loop();
}

void App::_initWindow()
{
    mWindow = Window();
    mWindow.init();
}

std::vector<const char *> App::_getExtensionNames()
{
    const char** winExts;
    uint32_t winExtCount;
    mWindow.vkGetRequiredExtensions(winExts, winExtCount);

    std::vector exts(winExts, winExts + winExtCount);
    exts.emplace_back(VK_KHR_PORTABILITY_ENUMERATION_EXTENSION_NAME);

    if (gEnableValidationLayers) {
        exts.emplace_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
    }

    return exts;
}

void App::_initVulkan()
{
    auto exts = _getExtensionNames();

    VkRendererConfig config{
        .requiredExtensions = exts,

        .enableValidationLayers = gEnableValidationLayers,
        .validationLayers = gValidationLayers,
        .deviceExtensions = gDeviceExtensions,

        .window = mWindow,
    };

    mVkRenderer.init(config);
}

void App::_loop()
{
    while (!mWindow.shouldClose()) {
        mWindow.update();
    }
}
