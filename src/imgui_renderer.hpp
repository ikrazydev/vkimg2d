#pragma once

#include <imgui.h>
#include <backends/imgui_impl_glfw.h>
#include <backends/imgui_impl_vulkan.h>

class ImGuiRenderer
{
public:
    ImGuiRenderer();
    ~ImGuiRenderer();

    void draw();
};
