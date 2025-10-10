#include "renderer.hpp"

#include <iostream>

VULKAN_HPP_DEFAULT_DISPATCH_LOADER_DYNAMIC_STORAGE;

VkRenderer::VkRenderer(VkRendererConfig config)
{
    VULKAN_HPP_DEFAULT_DISPATCHER.init();

    _createInstance(config);
    _printExtensions();

    if (config.enableValidationLayers) {
        _setupDebugMessenger();
    }

    _createSurface(config.window);

    mDevice.emplace(config, *this);

    _createSyncObjects(config);
}

VkRenderer::~VkRenderer()
{
    mDevice.reset();

    if (mSurface != VK_NULL_HANDLE) {
        mInstance->destroySurfaceKHR(mSurface);
    }
}

const vk::UniqueInstance& VkRenderer::getInstance() const
{
    return mInstance;
}

const vk::SurfaceKHR VkRenderer::getSurface() const
{
    return mSurface;
}

void VkRenderer::draw()
{
    const auto& device = mDevice.value();
    const auto& swapchain = device.getSwapchain();

    const auto& inFlightFence = mInFlightFences.value().getFence(mCurrentFrame);
    const auto& imageAvailableSemaphore = mImageAvailableSemaphores.value().getVkHandle(mCurrentFrame);

    inFlightFence.wait();
    inFlightFence.reset();

    uint32_t imageIndex = device.acquireNextImageKHR(imageAvailableSemaphore);

    const auto& renderedPerImageSemaphore = mRenderedPerImageSemaphores.value().getVkHandle(imageIndex);

    /*if (result == VK_ERROR_OUT_OF_DATE_KHR) {
        recreateSwapchain();
        return;
    }
    else if (result != VK_SUCCESS && result != VK_SUBOPTIMAL_KHR) {
        throw std::runtime_error("Failed to acquire swapchain image.");
    }*/

    inFlightFence.reset();

    mCommandBuffers.value().reset(mCurrentFrame);

    vk::SubmitInfo submitInfo{};
    vk::PipelineStageFlags waitMask = vk::PipelineStageFlagBits::eColorAttachmentOutput;
    submitInfo.setWaitSemaphores(imageAvailableSemaphore);
    submitInfo.setWaitDstStageMask({ waitMask });

    submitInfo.setCommandBuffers(mCommandBuffers.value().getVkHandle(mCurrentFrame));

    vk::Semaphore signalSemaphores[] = { renderedPerImageSemaphore };
    submitInfo.setSignalSemaphores(signalSemaphores);

    auto imageIndex = mDevice.value().getVkHandle().acquireNextImageKHR();

    if (vkQueueSubmit(mDevice.value().getQueueFamilies().graphicsFamily, 1, &submitInfo, inFlightFence) != VK_SUCCESS) {
        throw std::runtime_error("Failed to submit draw command buffer.");
    }

    vk::PresentInfoKHR presentInfo{};
    presentInfo.setWaitSemaphores(signalSemaphores);

    presentInfo.setSwapchains(swapchain);
    presentInfo.setPImageIndices(&imageIndex);

    presentInfo.setPResults(nullptr);

    result = vkQueuePresentKHR(mPresentQueue, &presentInfo);

    /*if (result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR || mFramebufferResized) {
        mFramebufferResized = false;
        recreateSwapchain();
    }
    else if (result != VK_SUCCESS) {
        throw std::runtime_error("Failed to present swapchain image.");
    }*/

    mCurrentFrame = (mCurrentFrame + 1) % mFramesInFlight;
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
    createInfo.setFlags(vk::InstanceCreateFlagBits::eEnumeratePortabilityKHR);
    createInfo.setPEnabledExtensionNames(config.requiredExtensions);

    vk::DebugUtilsMessengerCreateInfoEXT debugCreateInfo{};
    if (config.enableValidationLayers) {
        createInfo.setPEnabledLayerNames(config.validationLayers);

        _populateDebugMessengerCreateInfo(debugCreateInfo);
        createInfo.setPNext(&debugCreateInfo);
    } else {
        createInfo.setEnabledLayerCount(0);
    }

    mInstance = vk::createInstanceUnique(createInfo, nullptr);

    VULKAN_HPP_DEFAULT_DISPATCHER.init(mInstance.get());
}

VKAPI_ATTR vk::Bool32 VKAPI_CALL debugCallback(
    vk::DebugUtilsMessageSeverityFlagBitsEXT,
    vk::DebugUtilsMessageTypeFlagsEXT,
    const vk::DebugUtilsMessengerCallbackDataEXT* pCallbackData,
    void*)
{
    std::cerr << "Validation layer: " << pCallbackData->pMessage << "\n";

    return vk::False;
}

void VkRenderer::_printExtensions()
{
    auto exts = vk::enumerateInstanceExtensionProperties();

    std::cout << "Available extensions:\n";

    for (const auto& ext : exts) {
        std::cout << "\t" << ext.extensionName << "\n";
    }
}

void VkRenderer::_setupDebugMessenger()
{
    vk::DebugUtilsMessengerCreateInfoEXT createInfo;
    _populateDebugMessengerCreateInfo(createInfo);

    mDebugMessenger = mInstance->createDebugUtilsMessengerEXTUnique(createInfo);
}

void VkRenderer::_populateDebugMessengerCreateInfo(vk::DebugUtilsMessengerCreateInfoEXT& createInfo)
{
    createInfo.setMessageSeverity(
        // No info bit
        vk::DebugUtilsMessageSeverityFlagBitsEXT::eVerbose
        | vk::DebugUtilsMessageSeverityFlagBitsEXT::eWarning
        | vk::DebugUtilsMessageSeverityFlagBitsEXT::eError
    );
    createInfo.setMessageType(
        vk::DebugUtilsMessageTypeFlagBitsEXT::eGeneral
        | vk::DebugUtilsMessageTypeFlagBitsEXT::eValidation
        | vk::DebugUtilsMessageTypeFlagBitsEXT::ePerformance
    );
    createInfo.setPfnUserCallback(debugCallback);
    createInfo.setPUserData(nullptr);
}

void VkRenderer::_createSurface(const Window& window)
{
    VkSurfaceKHR rawSurface;
    if (window.vkCreateSurface(mInstance.get(), nullptr, &rawSurface) != VK_SUCCESS) {
        throw std::runtime_error("Failed to create window surface.");
    }

    mSurface = vk::SurfaceKHR(rawSurface);
}

void VkRenderer::_createCommandBuffers(const VkRendererConfig& config)
{
    CommandBufferConfig config = {
        .createCount = config.framesInFlight
    };

    mCommandBuffers.emplace(config);
}

void VkRenderer::_createSyncObjects(const VkRendererConfig& config)
{
    mImageAvailableSemaphores.emplace(mDevice, config.framesInFlight);
    mRenderedPerImageSemaphores.emplace(mDevice, mDevice.value().getSwapchain().getImageCount());
    mInFlightFences.emplace(mDevice, config.framesInFlight);

    mCurrentFrame = 0;
    mFramesInFlight = config.framesInFlight;
}
