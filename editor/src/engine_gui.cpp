#include "engine.hpp"
#include "include/core.hpp"
#include "engine_gui.hpp"

#include <entt/entt.hpp>
#include <fstream>

#include <iostream>


NullityEditor::State::State(Nullity::Engine& eng)
    : framebuffer(eng.state.initViewRes.x, eng.state.initViewRes.y) {}


void NullityEditor::UtilityKeybinds(Nullity::Engine& eng)
{
    if (isActionJustPressed("focus"))
    {
        eng.state.focus = !eng.state.focus;

        if (eng.state.focus)
            glfwSetInputMode(eng.window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
        else
            glfwSetInputMode(eng.window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
    }

    if (isActionJustPressed("wireframe"))
    {
        eng.state.wireframe = !eng.state.wireframe;

        if (eng.state.wireframe)
            glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
        else
            glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    }
}

void NullityEditor::Styling(float* _accent, float* _accent2, float* _bg1, float* _bg2)
{
    ImGuiStyle& style = ImGui::GetStyle();
    ImVec4 accent = ImVec4(_accent[0], _accent[1], _accent[2], _accent[3]);
    ImVec4 accent2 = ImVec4(_accent2[0], _accent2[1], _accent2[2], _accent2[3]); 
    ImVec4 bg1 = ImVec4(_bg1[0], _bg1[1], _bg1[2], _bg1[3]);
    ImVec4 bg2 = ImVec4(_bg2[0], _bg2[1], _bg2[2], _bg2[3]);

    style.WindowRounding = 5.0f;
    style.ChildRounding = 2.5f;
    style.FrameRounding = 2.5f;
    style.TabBarOverlineSize = 0.0f;
    
    // windows
    style.Colors[ImGuiCol_TitleBg] = bg2;
    style.Colors[ImGuiCol_TitleBgActive] = bg2;
    style.Colors[ImGuiCol_TabActive] = accent2;
    style.Colors[ImGuiCol_TabHovered] = accent2;
    style.Colors[ImGuiCol_TabDimmed] = accent;
    style.Colors[ImGuiCol_TabDimmedSelected] = accent;
    style.Colors[ImGuiCol_Tab] = accent;
    style.Colors[ImGuiCol_TabUnfocused] = accent; style.Colors[ImGuiCol_TabUnfocusedActive] = accent;
    style.Colors[ImGuiCol_TabSelected] = accent2;
    style.Colors[ImGuiCol_WindowBg] = bg1;
    style.Colors[ImGuiCol_PopupBg] = bg1;
    style.Colors[ImGuiCol_Border] = accent;
    style.Colors[ImGuiCol_ResizeGrip] = accent;
    style.Colors[ImGuiCol_ResizeGripActive] = accent2;
    style.Colors[ImGuiCol_ResizeGripHovered] = accent2;
    
    // menu bar
    style.Colors[ImGuiCol_MenuBarBg] = bg2;
    style.Colors[ImGuiCol_Header] = accent;
    style.Colors[ImGuiCol_HeaderHovered] = accent2;
    style.Colors[ImGuiCol_HeaderActive] = accent2;

    // list
    style.Colors[ImGuiCol_FrameBg] = bg2;

    // buttons
    style.Colors[ImGuiCol_Button] = accent;
    style.Colors[ImGuiCol_ButtonActive] = accent2;
    style.Colors[ImGuiCol_ButtonHovered] = accent2;
}

void NullityEditor::KeybindChangePopup()
{
    static std::string currentActionName;
    static std::vector<int> currentKeycodes;
    static bool changingKeybind = false;

    ImGui::Separator();
    ImGui::Text("Keymaps");
    ImGui::BeginChild("Keymaps");
    auto& bindings = getConfigKeymaps();
    for (auto& [actionName, keycodes] : bindings)
    {
        if (ImGui::Button(actionName.c_str()))
        {
            currentActionName = actionName;
            changingKeybind = true;
            ImGui::OpenPopup("Change Keymap?");
        }
    }
    if (changingKeybind)
    {
        currentKeycodes = bindings[currentActionName];
    }

    if(ImGui::BeginPopupModal("Change Keymap?", NULL, ImGuiWindowFlags_AlwaysAutoResize))
    {
        ImGui::Text("Changing action: %s", currentActionName.c_str());
        ImGui::Text("Press a key, then click 'add' or 'change' to assign the currently pressed key to that slot");
        
        ImGui::Separator();
        ImGui::Text("Press any key: %i", getCurrentScancodePressed());

        if(ImGui::BeginListBox("Current assigned keycodes"))
        {
            for (int i = 0; i < currentKeycodes.size(); i++)
            {
                int key = currentKeycodes[i];
                ImGui::Text("%i", key);
                ImGui::SameLine();

                ImGui::PushID(key + i);
                if (ImGui::Button("Change"))
                {
                    setConfigKeymap(currentActionName, false, getCurrentScancodePressed(), i);
                    reloadConfigKeymaps();
                }
                ImGui::SameLine();
                if (ImGui::Button("Remove"))
                {
                    removeConfigKeymap(currentActionName, i);
                    reloadConfigKeymaps();
                }
                ImGui::PopID();
            }
            if(ImGui::Button("Add"))
            {
                setConfigKeymap(currentActionName, true, getCurrentScancodePressed());
                reloadConfigKeymaps();
            }
            ImGui::EndListBox();
        }

        if (ImGui::Button("Close")) 
        { 
            ImGui::CloseCurrentPopup(); 
            changingKeybind = false;
        }
        ImGui::EndPopup();
    }
    ImGui::EndChild();
}

void NullityEditor::InfoWindow(float msPerFrame, int fps)
{
    ImGui::Begin("Info", NULL, ImGuiWindowFlags_None);
    ImGui::Text("ms per frame: %f", msPerFrame);
    ImGui::Text("fps: %i", fps);

    KeybindChangePopup();

    ImGui::End();
}

void NullityEditor::InspectorWindow(entt::registry& registry)
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

void NullityEditor::DebugOutputWindow()
{
    ImGui::Begin("Debug Output");

    std::ifstream fin;
    fin.open("DebugLog.txt");

    while(!fin.eof())
    {
        std::string line;
        std::getline(fin, line);
        ImGui::TextWrapped("%s", line.c_str());
    }

    fin.close();

    ImGui::End();
}
