#include "imgui_renderer.hpp"

#include <format>
#include <ranges>
#include <string>

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
    //auto& io = ImGui::GetIO();

    ImGui::Begin("Effects");

    const auto& regEffects = mAppData.registry.getEffects();
    static size_t curEffectIndex = 0;
    const auto& curEffect = regEffects.at(curEffectIndex);
    const char* curEffectLabel = curEffect.getDisplayName().c_str();

    if (ImGui::BeginCombo("Effects", curEffectLabel)) {
        for (size_t i = 0; i < regEffects.size(); i++) {
            bool isSelected = (curEffectIndex == i);
            const char* name = regEffects.at(i).getDisplayName().c_str();
            if (ImGui::Selectable(name, isSelected))
                curEffectIndex = i;
            if (isSelected)
                ImGui::SetItemDefaultFocus();
        }

        ImGui::EndCombo();
    }

    ImGui::SameLine();
    
    if (ImGui::Button("Add")) {
        mAppData.addEffect(&curEffect);
    }

    auto& effects = mAppData.effects;
    for (size_t i = 0; i < effects.size(); i++) {
        auto& effect = effects.at(i);

        std::string title = std::format("{} #{}", effect.effect->getDisplayName(), i + 1);

        if (!ImGui::CollapsingHeader(title.c_str(), ImGuiTreeNodeFlags_DefaultOpen)) continue;

        std::string checkboxText = std::format("Enabled##{}", i);
        ImGui::Checkbox(checkboxText.c_str(), &effect.enabled);

        for (auto& param : effect.params) {
            const auto& id = param.first;
            auto* value = &param.second;

            const auto* paramSpec = effect.effect->getParamById(id);

            std::string paramText = std::format("{}##{}", paramSpec->displayName, i);
            ImGui::SliderFloat(paramText.c_str(), value, paramSpec->min, paramSpec->max);
        }

        // if (ImGui::TreeNodeEx("Node Controls // TODO")) {
        //     ImGui::Button("Move Up");
        //     ImGui::SameLine();
        //     ImGui::Button("Move Down");
        //     ImGui::SameLine();
        //     ImGui::Button("Delete");

        //     ImGui::TreePop();
        // }
    }

    ImGui::Separator();

    static float mixValue = 100.0f;
    ImGui::SliderFloat("Master Mix", &mixValue, 0.0f, 100.0f, "%.2f");
    mAppData.mix = mixValue / 100.0f;

    ImGui::End();
}
