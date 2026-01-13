#include "renderer.hpp"

#include <iostream>
#include <utility>
#include <stdexcept>

#include <io/binary.hpp>
#include <io/path.hpp>

#include <imgui.h>
#include <backends/imgui_impl_glfw.h>
#include <backends/imgui_impl_vulkan.h>

VULKAN_HPP_DEFAULT_DISPATCH_LOADER_DYNAMIC_STORAGE;

VkRenderer::VkRenderer(VkRendererConfig config)
    : mAppData{ config.appData }
    , mWindow{ config.window }
{
    VULKAN_HPP_DEFAULT_DISPATCHER.init();

    _createInstance(config);
    _printExtensions();

    if (config.enableValidationLayers) {
        _setupDebugMessenger();
    }

    _createSurface(config.window);

    mDevice.emplace(config, *this);

    _createRenderpass();
    _createFramebuffers();
    _createCommandPool();
    _createBuffers(config);
    _createTextures(config);
    _createDescriptorLayouts(config);
    _createDescriptorSets(config);
    _createPipelines();
    _setupImGui(config);
    _createCommandBuffers(config);
    _createSyncObjects(config);
}

const vk::UniqueInstance& VkRenderer::getInstance() const noexcept
{
    return mInstance;
}

vk::SurfaceKHR VkRenderer::getSurface() const noexcept
{
    return mSurface->getVkHandle();
}

void VkRenderer::draw()
{
    mImGuiRenderer->draw();

    const auto& swapchain = mDevice->getSwapchain();

    const auto& inFlightFence = mInFlightFences->getFence(mCurrentFrame);
    const auto& imageAvailableSemaphore = mImageAvailableSemaphores->getVkHandle(mCurrentFrame);

    inFlightFence.wait();
    inFlightFence.reset();

    auto nextImageKHR = mDevice->acquireNextImageKHR(imageAvailableSemaphore);

    if (nextImageKHR.result == vk::Result::eErrorOutOfDateKHR) {
        _recreateSwapchain();
        return;
    }
    else if (nextImageKHR.result != vk::Result::eSuccess && nextImageKHR.result != vk::Result::eSuboptimalKHR) {
        throw std::runtime_error("Failed to acquire swapchain image.");
    }

    uint32_t imageIndex = nextImageKHR.value;

    const auto& renderedPerImageSemaphore = mRenderedPerImageSemaphores->getVkHandle(imageIndex);

    inFlightFence.reset();

    mCommandBuffers->reset(mCurrentFrame);
    mCommandBuffers->record(mCurrentFrame, imageIndex);

    vk::SubmitInfo submitInfo{};
    vk::PipelineStageFlags waitMask = vk::PipelineStageFlagBits::eColorAttachmentOutput;
    submitInfo.setWaitSemaphores(imageAvailableSemaphore);
    submitInfo.setWaitDstStageMask({ waitMask });

    const auto commandBuffer = mCommandBuffers->getVkHandle(mCurrentFrame);
    submitInfo.setCommandBuffers(commandBuffer);

    vk::Semaphore signalSemaphores[] = { renderedPerImageSemaphore };
    submitInfo.setSignalSemaphores(signalSemaphores);

    mDevice->getGraphicsQueue().submit(submitInfo, inFlightFence.getVkHandle());

    vk::PresentInfoKHR presentInfo{};
    presentInfo.setWaitSemaphores(signalSemaphores);

    const auto swapchainHandle = swapchain.getVkHandle();
    presentInfo.setSwapchains(swapchainHandle);
    presentInfo.setPImageIndices(&imageIndex);

    presentInfo.setPResults(nullptr);

    auto result = mDevice->getPresentQueue().presentKHR(presentInfo);

    if (result == vk::Result::eErrorOutOfDateKHR || result == vk::Result::eSuboptimalKHR || mWindow.mFramebufferResized) {
        mWindow.mFramebufferResized = false;
        _recreateSwapchain();
    }
    else if (result != vk::Result::eSuccess) {
        throw std::runtime_error("Failed to present swapchain image.");
    }

    mCurrentFrame = (mCurrentFrame + 1) % mFramesInFlight;
}

void VkRenderer::cleanup()
{
    mDevice.value().getVkHandle().waitIdle();
}

void VkRenderer::_createInstance(const VkRendererConfig& config)
{
    vk::ApplicationInfo appInfo{};
    appInfo.setPApplicationName("VkImg2D");
    appInfo.setApplicationVersion(VK_MAKE_VERSION(0, 1, 0));
    appInfo.setPEngineName("Custom");
    appInfo.setEngineVersion(VK_MAKE_VERSION(1, 0, 0));
    appInfo.setApiVersion(VK_API_VERSION_1_4);

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
    mSurface.emplace(mInstance.get(), window);
}

void VkRenderer::_createRenderpass()
{
    RenderpassConfig config = {
        .format = mDevice->getSwapchain().getSurfaceFormat().format,
        .colorLoadOp = vk::AttachmentLoadOp::eClear,
    };

    mRenderpass.emplace(mDevice.value(), config);
}

void VkRenderer::_createFramebuffers()
{
    const auto& swapchain = mDevice->getSwapchain();
    auto count = swapchain.getImageCount();

    mFramebuffers.clear();
    mFramebuffers.reserve(count);

    for (size_t i = 0; i < count; i++) {
        std::vector attachments{
            swapchain.getImageView(i),
        };

        mFramebuffers.emplace_back(mRenderpass.value(), attachments, swapchain.getExtent());
    }
}

void VkRenderer::_createCommandPool()
{
    CommandPoolConfig config = {
        .queueFamilyIndex = mDevice->getQueueFamilies().graphicsAndComputeFamily.value(),
    };

    mCommandPool.emplace(mDevice.value(), config);
}

void VkRenderer::_createBuffers(const VkRendererConfig& config)
{
    mVertexBuffer.emplace(Buffer::createVertex(mDevice.value(), mCommandPool.value(), config.vertices));
    mIndexBuffer.emplace(Buffer::createIndex(mDevice.value(), mCommandPool.value(), config.indices));
}

void VkRenderer::_createTextures(const VkRendererConfig& config)
{
    auto image = Image{ Paths::Samples / "sculpture_statue.jpg" };
    auto loadedImage = image.load();

    TextureImageConfig imageConfig = {
        .commandPool = mCommandPool.value(),
        .image = loadedImage,

        .type = TextureImageType::Sampled,
    };

    mTexture.emplace(mDevice.value(), imageConfig);

    SamplerConfig samplerConfig = {};
    mSampler.emplace(mDevice.value(), samplerConfig);

    ComputeImageConfig pingPongConfig = {
        .commandPool = mCommandPool.value(),
        .width = static_cast<uint32_t>(loadedImage.texWidth),
        .height = static_cast<uint32_t>(loadedImage.texHeight),
    };

    mImages.clear();
    mImages.reserve(config.framesInFlight);

    for (size_t i = 0; i < config.framesInFlight; i++) {
        mImages.emplace_back(
            mTexture.value(),
            TextureImage{ mDevice.value(), pingPongConfig },
            TextureImage{ mDevice.value(), pingPongConfig }
        );
    }
}

void VkRenderer::_createDescriptorLayouts(const VkRendererConfig& config)
{
    std::vector<DescriptorLayoutBindingConfig> fragmentBindings{
        DescriptorLayoutBindingConfig{
            .binding = 0U,
            .type = vk::DescriptorType::eCombinedImageSampler,
        },
        DescriptorLayoutBindingConfig{
            .binding = 1U,
            .type = vk::DescriptorType::eCombinedImageSampler,
        },
    };

    DescriptorLayoutConfig fragmentLayoutConfig = {
        .bindings = fragmentBindings,
        .stages = vk::ShaderStageFlagBits::eFragment,
    };

    mFragmentDescriptorLayout.emplace(mDevice.value(), fragmentLayoutConfig);

    std::vector<DescriptorLayoutBindingConfig> samplerBindings{
        DescriptorLayoutBindingConfig{
            .binding = 0U,
            .type = vk::DescriptorType::eCombinedImageSampler,
        },
        DescriptorLayoutBindingConfig{
            .binding = 1U,
            .type = vk::DescriptorType::eStorageImage,
        },
    };

    DescriptorLayoutConfig samplerLayoutConfig = {
        .bindings = samplerBindings,
        .stages = vk::ShaderStageFlagBits::eCompute,
    };

    mSamplerDescriptorLayout.emplace(mDevice.value(), samplerLayoutConfig);

    std::vector<DescriptorLayoutBindingConfig> effectBindings{
        DescriptorLayoutBindingConfig{
            .binding = 0U,
            .type = vk::DescriptorType::eStorageImage,
        },
        DescriptorLayoutBindingConfig{
            .binding = 1U,
            .type = vk::DescriptorType::eStorageImage,
        },
    };

    DescriptorLayoutConfig effectLayoutConfig = {
        .bindings = effectBindings,
        .stages = vk::ShaderStageFlagBits::eCompute,
    };

    mEffectDescriptorLayout.emplace(mDevice.value(), effectLayoutConfig);
}

void VkRenderer::_createDescriptorSets(const VkRendererConfig& config)
{
    std::vector<DescriptorPoolSize> poolSizes;
    poolSizes.reserve(2U);

    DescriptorPoolSize samplerPoolSize{
        .type = vk::DescriptorType::eCombinedImageSampler,
        .count = static_cast<uint32_t>(config.framesInFlight) * 10,
    };
    DescriptorPoolSize storagePoolSize{
        .type = vk::DescriptorType::eStorageImage,
        .count = static_cast<uint32_t>(config.framesInFlight) * 150,
    };

    poolSizes.push_back(samplerPoolSize);
    poolSizes.push_back(storagePoolSize);

    DescriptorPoolConfig poolConfig{
        .sizes = poolSizes,
        .maxSets = static_cast<uint32_t>(config.framesInFlight) * 200 + IMGUI_IMPL_VULKAN_MINIMUM_IMAGE_SAMPLER_POOL_SIZE,
    };

    mDescriptorPool.emplace(mDevice.value(), poolConfig);

    mDescriptors.clear();
    mDescriptors.reserve(config.framesInFlight);

    for (size_t i = 0; i < config.framesInFlight; i++)
    {
        const auto& images = mImages.at(i);

        std::vector<DescriptorSetImage> samplerImages;
        samplerImages.reserve(2);
        samplerImages.push_back(DescriptorSetImage{
            .binding = 0U,
            .texture = mTexture.value(),
            .sampler = &mSampler.value(),
            .layout = vk::ImageLayout::eShaderReadOnlyOptimal,
            .descriptorType = vk::DescriptorType::eCombinedImageSampler,
        });
        samplerImages.push_back(DescriptorSetImage{
            .binding = 1U,
            .texture = images.ping,
            .layout = vk::ImageLayout::eGeneral,
            .descriptorType = vk::DescriptorType::eStorageImage,
        });

        DescriptorSetConfig samplerConfig = {
            .descriptorLayout = mSamplerDescriptorLayout.value(),
            .descriptorPool = mDescriptorPool.value()
        };

        DescriptorSet sampler{ mDevice.value(), samplerConfig };
        sampler.update(DescriptorUpdateConfig{ .images = samplerImages });

        std::vector<DescriptorSetImage> computeABImages;
        computeABImages.reserve(2);
        computeABImages.push_back(DescriptorSetImage{
            .binding = 0U,
            .texture = images.ping,
            .layout = vk::ImageLayout::eGeneral,
            .descriptorType = vk::DescriptorType::eStorageImage,
        });
        computeABImages.push_back(DescriptorSetImage{
            .binding = 1U,
            .texture = images.pong,
            .layout = vk::ImageLayout::eGeneral,
            .descriptorType = vk::DescriptorType::eStorageImage,
        });

        DescriptorSetConfig computeABConfig = {
            .descriptorLayout = mEffectDescriptorLayout.value(),
            .descriptorPool = mDescriptorPool.value()
        };

        DescriptorSet computeAB{ mDevice.value(), computeABConfig };
        computeAB.update(DescriptorUpdateConfig{ .images = computeABImages });

        std::vector<DescriptorSetImage> computeBAImages;
        computeBAImages.reserve(2);
        computeBAImages.push_back(DescriptorSetImage{
            .binding = 0U,
            .texture = images.pong,
            .layout = vk::ImageLayout::eGeneral,
            .descriptorType = vk::DescriptorType::eStorageImage,
        });
        computeBAImages.push_back(DescriptorSetImage{
            .binding = 1U,
            .texture = images.ping,
            .layout = vk::ImageLayout::eGeneral,
            .descriptorType = vk::DescriptorType::eStorageImage,
        });

        DescriptorSetConfig computeBAConfig = {
            .descriptorLayout = mEffectDescriptorLayout.value(),
            .descriptorPool = mDescriptorPool.value()
        };

        DescriptorSet computeBA{ mDevice.value(), computeBAConfig };
        computeBA.update(DescriptorUpdateConfig{ .images = computeBAImages });

        std::vector<DescriptorSetImage> graphicsAImages;
        graphicsAImages.reserve(2);
        graphicsAImages.push_back(DescriptorSetImage{
            .binding = 0U,
            .texture = images.ping,
            .sampler = &mSampler.value(),
            .layout = vk::ImageLayout::eShaderReadOnlyOptimal,
            .descriptorType = vk::DescriptorType::eCombinedImageSampler,
        });
        graphicsAImages.push_back(DescriptorSetImage{
            .binding = 1U,
            .texture = mTexture.value(),
            .sampler = &mSampler.value(),
            .layout = vk::ImageLayout::eShaderReadOnlyOptimal,
            .descriptorType = vk::DescriptorType::eCombinedImageSampler,
        });

        DescriptorSetConfig graphicsAConfig = {
            .descriptorLayout = mFragmentDescriptorLayout.value(),
            .descriptorPool = mDescriptorPool.value(),
        };

        DescriptorSet graphicsA{ mDevice.value(), graphicsAConfig };
        graphicsA.update(DescriptorUpdateConfig{ .images = graphicsAImages });

        std::vector<DescriptorSetImage> graphicsBImages;
        graphicsBImages.reserve(2);
        graphicsBImages.push_back(DescriptorSetImage{
            .binding = 0U,
            .texture = images.pong,
            .sampler = &mSampler.value(),
            .layout = vk::ImageLayout::eShaderReadOnlyOptimal,
            .descriptorType = vk::DescriptorType::eCombinedImageSampler,
        });
        graphicsBImages.push_back(DescriptorSetImage{
            .binding = 1U,
            .texture = mTexture.value(),
            .sampler = &mSampler.value(),
            .layout = vk::ImageLayout::eShaderReadOnlyOptimal,
            .descriptorType = vk::DescriptorType::eCombinedImageSampler,
        });

        DescriptorSetConfig graphicsBConfig = {
            .descriptorLayout = mFragmentDescriptorLayout.value(),
            .descriptorPool = mDescriptorPool.value(),
        };

        DescriptorSet graphicsB{ mDevice.value(), graphicsBConfig };
        graphicsB.update(DescriptorUpdateConfig{ .images = graphicsBImages });

        mDescriptors.emplace_back(RenderDescriptorSet{
            .sampler = std::move(sampler),

            .computeAtoB = std::move(computeAB),
            .computeBtoA = std::move(computeBA),

            .graphicsA = std::move(graphicsA),
            .graphicsB = std::move(graphicsB),
        });
    }
}

void VkRenderer::_createPipelines()
{
    ComputePipelineConfig samplerConfig = {
        .shaderPath = BinaryReader::toShaderBinPath("sampler.spv"),
        .descriptorLayout = mSamplerDescriptorLayout.value(),
        .usePushConstants = false,
    };

    mSamplerPipeline.emplace(mDevice.value(), samplerConfig);

    GraphicsPipelineConfig graphicsConfig = {
        .vertexShaderPath = BinaryReader::toShaderBinPath("vertex.spv"),
        .fragmentShaderPath = BinaryReader::toShaderBinPath("fragment.spv"),

        .swapchainExtent = mDevice->getSwapchain().getExtent(),

        .renderpass = mRenderpass.value(),
        .subpass = 0,

        .descriptorLayout = mFragmentDescriptorLayout.value(),

        .usePushConstants = true,
        .pushConstantSize = sizeof(float),
    };

    mGraphicsPipeline.emplace(mDevice.value(), graphicsConfig);

    std::unordered_map<std::string, ComputePipeline> effectPipelines;

    for (const auto& effect : mAppData.registry.getEffects()) {
        const auto& id = effect.getId();
        const auto& shaderPath = effect.getShaderPath();

        uint32_t pushConstantSize = effect.getParams().size() * sizeof(float);

        ComputePipelineConfig pipeConfig = {
            .shaderPath = shaderPath,
            .descriptorLayout = mEffectDescriptorLayout.value(),
            .usePushConstants = pushConstantSize > 0U,
            .pushConstantSize = pushConstantSize,
        };

        effectPipelines.try_emplace(id, mDevice.value(), pipeConfig);
    }

    mPipelineSet.emplace(std::move(effectPipelines));
}

void VkRenderer::_setupImGui(const VkRendererConfig& config)
{
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();

    auto& io = ImGui::GetIO();
    io.Fonts->AddFontDefault();
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
    io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
    io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;

    float xscale, yscale, pointscale;
    uint32_t width, height;
    config.window.getScaling(&xscale, &yscale);
    config.window.getPointScaling(&pointscale);
    config.window.getFramebufferSize(&width, &height);

    io.DisplayFramebufferScale = ImVec2{ xscale, yscale };
    io.DisplaySize = ImVec2{ (float)width, (float)height };
    io.FontGlobalScale = yscale / pointscale;

    ImGui::StyleColorsDark();

    ImGui_ImplGlfw_InitForVulkan(config.window.getGLFWHandle(), true);

    auto families = mDevice->getQueueFamilies();

    ImGui_ImplVulkan_InitInfo initInfo{};
    initInfo.ApiVersion = VK_API_VERSION_1_4;
    initInfo.Instance = mInstance.get();
    initInfo.PhysicalDevice = mDevice->getPhysicalDevice(),
    initInfo.Device = mDevice->getVkHandle();
    initInfo.QueueFamily = families.graphicsAndComputeFamily.value();
    initInfo.Queue = mDevice->getGraphicsQueue();
    initInfo.PipelineCache = nullptr;
    initInfo.DescriptorPool = mDescriptorPool->getVkHandle();
    initInfo.MinImageCount = config.framesInFlight;
    initInfo.ImageCount = mDevice->getSwapchain().getImageCount();
    initInfo.Allocator = nullptr;
    initInfo.CheckVkResultFn = nullptr;

    ImGui_ImplVulkan_PipelineInfo pipelineInfo{};
    pipelineInfo.RenderPass = mRenderpass->getVkHandle();
    pipelineInfo.Subpass = 0U;
    pipelineInfo.MSAASamples = VK_SAMPLE_COUNT_1_BIT;
    
    initInfo.PipelineInfoMain = pipelineInfo;

    ImGui_ImplVulkan_Init(&initInfo);

    mImGuiRenderer.emplace(mAppData);
}

void VkRenderer::_createCommandBuffers(const VkRendererConfig& rendererConfig)
{
    CommandBufferConfig config = {
        .appData = mAppData,

        .commandPool = mCommandPool.value(),
        .renderpass = mRenderpass.value(),
        .framebuffers = &mFramebuffers,

        .renderDescriptors = mDescriptors,
        .renderImages = mImages,
        .samplerPipeline = mSamplerPipeline.value(),
        .graphicsPipeline = mGraphicsPipeline.value(),
        .pipelineSet = mPipelineSet.value(),

        .extent = mDevice->getSwapchain().getExtent(),

        .createCount = rendererConfig.framesInFlight,

        .vertexBuffer = mVertexBuffer.value(),
        .indexBuffer = mIndexBuffer.value(),

        .drawIndexCount = static_cast<uint32_t>(rendererConfig.indices.size()),
        .drawInstanceCount = 1U,
    };

    mCommandBuffers.emplace(mDevice.value(), config);
}

void VkRenderer::_createSyncObjects(const VkRendererConfig& config)
{
    mImageAvailableSemaphores.emplace(mDevice.value(), config.framesInFlight);
    mRenderedPerImageSemaphores.emplace(mDevice.value(), mDevice.value().getSwapchain().getImageCount());
    mInFlightFences.emplace(mDevice.value(), FenceConfig{ .signaled = true }, config.framesInFlight);

    mCurrentFrame = 0;
    mFramesInFlight = config.framesInFlight;
}

void VkRenderer::_recreateSwapchain()
{
    uint32_t width, height;
    mWindow.getFramebufferSize(&width, &height);

    while (width == 0 || height == 0) {
        mWindow.getFramebufferSize(&width, &height);
        mWindow.waitForEvents();
    }

    mDevice->getVkHandle().waitIdle();
    mDevice->recreateSwapchain();

    _createFramebuffers();

    mCommandBuffers->updateFramebuffers(&mFramebuffers, mDevice->getSwapchain().getExtent());
}
