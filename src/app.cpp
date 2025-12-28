#include "app.hpp"

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

static const uint32_t gMaxFramesInFlight = 2;

static const std::vector<Vertex> gVertices = {
    {{ -1.0f, -1.0f }, { 1.0f, 0.0f, 0.0f }, { 0.0f, 0.0f }},
    {{ 1.0f, -1.0f }, { 0.0f, 1.0f, 0.0f }, { 1.0f, 0.0f }},
    {{ 1.0f, 1.0f }, { 0.0f, 0.0f, 1.0f }, { 1.0f, 1.0f }},
    {{ -1.0f, 1.0f }, { 1.0f, 1.0f, 1.0f }, { 0.0f, 1.0f }},
};

static const std::vector<uint32_t> gIndices = {
    0, 1, 2, 2, 3, 0,
};

App::App()
    : mWindow{ WindowConfig{} }
{
    _createWindow();
    _initVulkan();
}

void App::run()
{
    _loop();

    mVkRenderer->cleanup();
}

void App::_createWindow()
{
    mWindow.create();
}

std::vector<const char*> App::_getExtensionNames() const
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
        .appData = mAppData,

        .requiredExtensions = exts,

        .enableValidationLayers = gEnableValidationLayers,
        .validationLayers = gValidationLayers,
        .deviceExtensions = gDeviceExtensions,

        .vertices = gVertices,
        .indices = gIndices,

        .framesInFlight = gMaxFramesInFlight,

        .window = mWindow,
    };

    mVkRenderer.emplace(config);
}

void App::_loop()
{
    while (!mWindow.shouldClose()) {
        mWindow.update();
        mVkRenderer->draw();
    }
}
