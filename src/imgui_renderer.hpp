#pragma once

#include <string>

#include <app_data.hpp>

#include <imgui.h>
#include <imgui/backends/imgui_impl_glfw.h>
#include <imgui/backends/imgui_impl_vulkan.h>

class ImGuiRenderer
{
public:
    ImGuiRenderer(AppData& appData);
    ~ImGuiRenderer();

    void draw();
private:
    static std::string _toUniqueId(std::string_view str, const size_t index);

    AppData& mAppData;
};
