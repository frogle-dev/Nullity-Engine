#include "inspector.hpp"

#include "imgui/imgui.h"
#include "imgui/backends/imgui_impl_glfw.h"
#include "imgui/backends/imgui_impl_opengl3.h"

#include <entt/entt.hpp>

#include "components.hpp"


void InspectorWindow(entt::registry& registry)
{
    static std::string inspectorCurrent = "None";

    ImGui::Begin("Inspector", NULL, ImGuiWindowFlags_None);

    ImGui::Text("Entity: ");
    
    auto view = registry.view<DisplayName>();
    if (ImGui::BeginCombo("##", inspectorCurrent.c_str()))
    {
        for (auto [entity, cmp_name] : view.each())
        {
            bool selected = (inspectorCurrent == cmp_name.name);
            if (ImGui::Selectable(cmp_name.name.c_str(), selected))
            {
                inspectorCurrent = cmp_name.name;
            }
            if (selected)
            {
                ImGui::SetItemDefaultFocus();
            }
        }

        ImGui::EndCombo();
    }

    ImGui::End();
}