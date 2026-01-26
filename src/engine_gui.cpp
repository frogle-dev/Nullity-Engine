#include "imgui/imgui.h"
#include "imgui/backends/imgui_impl_glfw.h"
#include "imgui/backends/imgui_impl_opengl3.h"

#include "engine_gui.hpp"
#include "keymap.hpp"

#include <entt/entt.hpp>


std::string currentActionName;
std::vector<int> currentKeycodes;
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
        ImGui::Text("Press any key: %i", currentScancodePress);

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
                    setConfigKeymap(currentActionName, false, currentScancodePress, i);
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
                setConfigKeymap(currentActionName, true, currentScancodePress);
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