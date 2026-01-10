#include "imgui_renderer.hpp"

#include <format>
#include <ranges>
#include <string>
#include <optional>

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

    std::optional<size_t> queueDelete;
    std::optional<size_t> queueMoveUp;
    std::optional<size_t> queueMoveDown;

    for (size_t i = 0; i < effects.size(); i++) {
        auto& effect = effects.at(i);

        std::string title = std::format("{} #{}", effect.effect->getDisplayName(), i + 1);

        if (!ImGui::CollapsingHeader(title.c_str(), ImGuiTreeNodeFlags_DefaultOpen)) continue;

        auto checkboxText = _toUniqueId("Enabled", i);
        ImGui::Checkbox(checkboxText.c_str(), &effect.enabled);

        for (auto& param : effect.params) {
            const auto& id = param.first;
            auto* value = &param.second;

            const auto* paramSpec = effect.effect->getParamById(id);

            std::string paramText = std::format("{}##{}", paramSpec->displayName, i);
            ImGui::SliderFloat(paramText.c_str(), value, paramSpec->min, paramSpec->max);
        }

        auto nodeText = _toUniqueId("Node", i);
        auto moveUpText = _toUniqueId("Move Up", i);
        auto moveDownText  = _toUniqueId("Move Down", i);
        auto deleteText  = _toUniqueId("Delete", i);

        if (ImGui::TreeNodeEx(nodeText.c_str(), ImGuiTreeNodeFlags_DefaultOpen)) {
            if (ImGui::Button(moveUpText.c_str())) {
                queueMoveUp = i;
            }

            ImGui::SameLine();

            if (ImGui::Button(moveDownText.c_str())) {
                queueMoveDown = i;
            }

            ImGui::SameLine();

            if (ImGui::Button(deleteText.c_str())) {
                queueDelete = i;
            }

            ImGui::TreePop();
        }
    }

    ImGui::Separator();

    static float mixValue = 100.0f;
    ImGui::SliderFloat("Master Mix", &mixValue, 0.0f, 100.0f, "%.2f");
    mAppData.mix = mixValue / 100.0f;

    ImGui::End();

    if (queueMoveUp.has_value()) {
        mAppData.moveUpEffect(queueMoveUp.value());
    }
    if (queueMoveDown.has_value()) {
        mAppData.moveDownEffect(queueMoveDown.value());
    }
    if (queueDelete.has_value()) {
        mAppData.deleteEffect(queueDelete.value());
    }
}

std::string ImGuiRenderer::_toUniqueId(std::string_view str, const size_t index)
{
    return std::format("{}##{}", str, index);
}
