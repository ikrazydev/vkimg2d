#include "imgui_renderer.hpp"

ImGuiRenderer::ImGuiRenderer(AppData& appData)
    : mAppData{ appData }
{
}

ImGuiRenderer::~ImGuiRenderer()
{
    ImGui_ImplVulkan_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();
}

void ImGuiRenderer::draw()
{
    ImGui_ImplVulkan_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();

    // TODO: support fullscreen docking in the future

    auto& io = ImGui::GetIO();

    ImGui::Begin("Effects");

    const char* items[] = { "Grayscale", "Sepia", "Invert" };
    static const char* currentEffect = items[0];

    if (ImGui::BeginCombo("Effects", currentEffect)) {
        for (int n = 0; n < IM_ARRAYSIZE(items); n++) {
            bool isSelected = (currentEffect == items[n]);
            if (ImGui::Selectable(items[n], isSelected))
                currentEffect = items[n];
            if (isSelected)
                ImGui::SetItemDefaultFocus();
        }

        ImGui::EndCombo();
    }

    ImGui::SameLine();
    ImGui::Button("Add");

    if (ImGui::CollapsingHeader("Grayscale #1", ImGuiTreeNodeFlags_DefaultOpen)) {
        static bool enabled = true;
        ImGui::Checkbox("Enabled", &enabled);
    }

    ImGui::Separator();

    static float mixValue = 100.0f;
    ImGui::SliderFloat("Master Mix", &mixValue, 0.0f, 100.0f, "%.2f");
    mAppData.mix = mixValue / 100.0f;

    ImGui::End();
}
