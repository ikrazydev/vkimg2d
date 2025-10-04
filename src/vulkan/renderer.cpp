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
    vkWaitForFences(mDevice, 1, &mInFlightFences[mCurrentFrame], VK_TRUE, UINT64_MAX);
    vkResetFences(mDevice, 1, &mInFlightFences[mCurrentFrame]);

    uint32_t imageIndex;
    auto result = vkAcquireNextImageKHR(mDevice, mSwapchain, UINT64_MAX, mImageAvailableSemaphores[mCurrentFrame], VK_NULL_HANDLE, &imageIndex);

    if (result == VK_ERROR_OUT_OF_DATE_KHR) {
        recreateSwapchain();
        return;
    }
    else if (result != VK_SUCCESS && result != VK_SUBOPTIMAL_KHR) {
        throw std::runtime_error("Failed to acquire swapchain image.");
    }

    vkResetFences(mDevice, 1, &mInFlightFences[mCurrentFrame]);

    vkResetCommandBuffer(mCommandBuffers[mCurrentFrame], 0);
    vkResetCommandBuffer(mImGuiCommandBuffers[mCurrentFrame], 0);
    recordCommandBuffers(mCommandBuffers[mCurrentFrame], mImGuiCommandBuffers[mCurrentFrame], imageIndex);

    VkSubmitInfo submitInfo{};
    submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;

    VkSemaphore waitSemaphores[] = { mImageAvailableSemaphores[mCurrentFrame] };
    VkPipelineStageFlags waitStages[] = { VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT };
    submitInfo.waitSemaphoreCount = 1;
    submitInfo.pWaitSemaphores = waitSemaphores;
    submitInfo.pWaitDstStageMask = waitStages;

    submitInfo.commandBufferCount = 1;
    submitInfo.pCommandBuffers = &mCommandBuffers[mCurrentFrame];

    VkSemaphore signalSemaphores[] = { mRenderedPerImageSemaphores[imageIndex] };
    submitInfo.signalSemaphoreCount = 1;
    submitInfo.pSignalSemaphores = signalSemaphores;

    if (vkQueueSubmit(mGraphicsQueue, 1, &submitInfo, mInFlightFences[mCurrentFrame]) != VK_SUCCESS) {
        throw std::runtime_error("Failed to submit draw command buffer.");
    }

    VkPresentInfoKHR presentInfo{};
    presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
    presentInfo.waitSemaphoreCount = 1;
    presentInfo.pWaitSemaphores = signalSemaphores;

    VkSwapchainKHR swapchains[] = { mSwapchain };
    presentInfo.swapchainCount = 1;
    presentInfo.pSwapchains = swapchains;
    presentInfo.pImageIndices = &imageIndex;

    presentInfo.pResults = nullptr;

    result = vkQueuePresentKHR(mPresentQueue, &presentInfo);

    if (result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR || mFramebufferResized) {
        mFramebufferResized = false;
        recreateSwapchain();
    }
    else if (result != VK_SUCCESS) {
        throw std::runtime_error("Failed to present swapchain image.");
    }

    mCurrentFrame = (mCurrentFrame + 1) % MAX_FRAMES_IN_FLIGHT;
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

void VkRenderer::_createSyncObjects(const VkRendererConfig& config)
{
    mImageAvailableSemaphores.resize(config.framesInFlight);
    mRenderedPerImageSemaphores.resize(mDevice.value().getSwapchain().getImageCount());
    mInFlightFences.resize(config.framesInFlight);

    vk::SemaphoreCreateInfo semaphoreInfo{};

    vk::FenceCreateInfo fenceInfo{};
    fenceInfo.flags = vk::FenceCreateFlagBits::eSignaled;

    for (size_t i = 0; i < config.framesInFlight; i++) {
        if (vkCreateSemaphore(mDevice, &semaphoreInfo, nullptr, &mImageAvailableSemaphores[i]) != VK_SUCCESS
            || vkCreateFence(mDevice, &fenceInfo, nullptr, &mInFlightFences[i]) != VK_SUCCESS)
        {
            throw std::runtime_error("Failed to create synchronization objects.");
        }
    }

    for (size_t i = 0; i < mRenderedPerImageSemaphores.size(); i++) {
        if (vkCreateSemaphore(mDevice, &semaphoreInfo, nullptr, &mRenderedPerImageSemaphores[i]) != VK_SUCCESS) {
            throw std::runtime_error("Failed to create synchronization objects.");
        }
    }
}
