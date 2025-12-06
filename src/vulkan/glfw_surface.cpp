#include "glfw_surface.hpp"

#include <stdexcept>

GLFWVkSurface::GLFWVkSurface(const vk::Instance instance, const Window& window)
    : mInstance{ instance }
{
    VkSurfaceKHR rawSurface;
    if (window.vkCreateSurface(instance, nullptr, &rawSurface) != VK_SUCCESS) {
        throw std::runtime_error("Failed to create window surface.");
    }

    mSurface = vk::SurfaceKHR(rawSurface);
}

GLFWVkSurface::~GLFWVkSurface()
{
    if (mSurface != nullptr) {
        mInstance.destroySurfaceKHR(mSurface);
    }
}

const vk::SurfaceKHR GLFWVkSurface::getVkHandle() const
{
    return mSurface;
}
