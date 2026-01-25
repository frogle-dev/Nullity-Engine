#pragma once

#include "imgui/imgui.h"
#include "imgui/backends/imgui_impl_glfw.h"
#include "imgui/backends/imgui_impl_opengl3.h"

#include "keymap.hpp"

#include <entt/entt.hpp>


std::string cur_actionName;
std::vector<int> cur_keycodes;
bool changingKeybind = false;
void KeybindChangePopup()
{
    ImGui::Separator();
    ImGui::Text("Keymaps");
    ImGui::BeginChild("Keymaps");
    auto& bindings = getConfigKeymaps();
    for (auto& [actionName, keycodes] : bindings)
    {
        if (ImGui::Button(actionName.c_str()))
        {
            cur_actionName = actionName;
            changingKeybind = true;
            ImGui::OpenPopup("Change Keymap?");
        }
    }
    if (changingKeybind)
    {
        cur_keycodes = bindings[cur_actionName];
    }

    if(ImGui::BeginPopupModal("Change Keymap?", NULL, ImGuiWindowFlags_AlwaysAutoResize))
    {
        ImGui::Text("Changing action: %s", cur_actionName.c_str());
        ImGui::Text("Press a key, then click 'add' or 'change' to assign the currently pressed key to that slot");
        
        ImGui::Separator();
        ImGui::Text("Press any key: %i", currentScancodePress);

        if(ImGui::BeginListBox("Current assigned keycodes"))
        {
            for (int i = 0; i < cur_keycodes.size(); i++)
            {
                int key = cur_keycodes[i];
                ImGui::Text("%i", key);
                ImGui::SameLine();

                ImGui::PushID(key + i);
                if (ImGui::Button("Change"))
                {
                    setConfigKeymap(cur_actionName, false, currentScancodePress, i);
                    reloadConfigKeymaps();
                }
                ImGui::SameLine();
                if (ImGui::Button("Remove"))
                {
                    removeConfigKeymap(cur_actionName, i);
                    reloadConfigKeymaps();
                }
                ImGui::PopID();
            }
            if(ImGui::Button("Add"))
            {
                setConfigKeymap(cur_actionName, true, currentScancodePress);
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

void InfoWindow(float msPerFrame, int fps)
{
    ImGui::Begin("Info", NULL, ImGuiWindowFlags_None);
    ImGui::Text("ms per frame: %f", msPerFrame);
    ImGui::Text("fps: %i", fps);

    KeybindChangePopup();

    ImGui::End();
}