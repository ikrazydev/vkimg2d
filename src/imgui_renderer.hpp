#pragma once

#include <app_data.hpp>

#include <imgui.h>
#include <backends/imgui_impl_glfw.h>
#include <backends/imgui_impl_vulkan.h>

class ImGuiRenderer
{
public:
    ImGuiRenderer(AppData& appData);
    ~ImGuiRenderer();

    void draw();
private:
    AppData& mAppData;
};
