#include <vulkan/renderer.hpp>

void VkRenderer::init(VkRendererConfig config)
{
    _createInstance(config);
}

void VkRenderer::_createInstance(const VkRendererConfig& config)
{
    vk::ApplicationInfo appInfo{};
    appInfo.setPApplicationName("VkImg2D");
    appInfo.setApplicationVersion(VK_MAKE_VERSION(0, 1, 0));
    appInfo.setPEngineName("Custom");
    appInfo.setEngineVersion(VK_MAKE_VERSION(1, 0, 0));
    appInfo.setApiVersion(VK_API_VERSION_1_3);

    vk::InstanceCreateInfo createInfo{};
    createInfo.setPApplicationInfo(&appInfo);
    createInfo.setFlags(vk::InstanceCreateFlags(VK_INSTANCE_CREATE_ENUMERATE_PORTABILITY_BIT_KHR));
    createInfo.setEnabledExtensionCount(config.requiredExtensionCount);
    createInfo.setPpEnabledExtensionNames(config.requiredExtensions);

    mInstance = vk::createInstanceUnique(createInfo, nullptr);
}
