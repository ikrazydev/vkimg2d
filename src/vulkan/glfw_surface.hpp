#pragma once

#include <window.hpp>

#include <vulkan/include.hpp>

class GLFWVkSurface
{
public:
    GLFWVkSurface(const vk::Instance instance, const Window& window);
    ~GLFWVkSurface();

    const vk::SurfaceKHR getVkHandle() const;
private:
    vk::Instance mInstance;
    vk::SurfaceKHR mSurface;
};
