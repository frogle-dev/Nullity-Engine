#include "glad.h"
#include <GLFW/glfw3.h>

#include "json.hpp"
#include <fstream>

#include "input.hpp"
#include "debugging.hpp"

#include <sstream>
#include <unordered_map>
#include <vector>

namespace N = Nullity;


std::unordered_map<std::string, std::vector<int>> bindings;
std::unordered_map<std::string, N::Input::ActionState> actions;
int currentScancodePress;

std::ifstream keymapJson;
nlohmann::json data;

void N::Input::InputInit()
{
    keymapJson.open("config/keymaps.json");
    data = nlohmann::json::parse(keymapJson);
}

std::unordered_map<std::string, std::vector<int>>& N::Input::GetConfigKeymaps() { return bindings; }
int N::Input::CurrentScancodePressed() { return currentScancodePress; }

void N::Input::KeysRefresh() 
{
    for (auto& [name, state] : actions) 
    {
        state.justPressed = false;
        state.released = false;
    }
}


bool N::Input::IsActionPressed(const std::string& actionName) 
{
    return actions[actionName].pressed;
}

bool N::Input::IsActionJustPressed(const std::string& actionName) 
{
    return actions[actionName].justPressed;
}

bool N::Input::IsActionReleased(const std::string& actionName) 
{
    return actions[actionName].released;
}

// called by glfw key callback thing
void N::Input::ProcessKeyEvent(int scancode, int action) 
{
    currentScancodePress = scancode;

    for (auto& [actionName, scancodes] : bindings) 
    {
        for (int boundScancode : scancodes)
        {
            if (boundScancode == currentScancodePress) 
            {
                auto& state = actions[actionName];

                if (action == GLFW_PRESS) 
                {
                    if (!state.pressed)
                        state.justPressed = true;
                    state.pressed = true;
                } 
                else if (action == GLFW_RELEASE)
                {
                    if (state.pressed)
                        state.released = true;
                    state.pressed = false;
                }
            }
        }
    }
}

void N::Input::ReloadConfigKeymaps()
{
    bindings.clear();
    for (auto& [actionName, keycodes] : data.items())
    {
        std::vector<int> scancodes;
        for (int i : keycodes)
        {
            scancodes.push_back(glfwGetKeyScancode(i));
        }
        bindings[actionName] = scancodes;
    }
}

void N::Input::SetConfigKeymap(const std::string& actionName, bool addKeycode, int keycode, int index)
{
    if (addKeycode)
    {
        data[actionName].push_back(keycode);
    }
    else
    {
        data[actionName][index] = keycode;
    }

    std::ofstream out("../game_config/keymaps.json");
    out << data.dump(4);
}

void N::Input::RemoveConfigKeymap(const std::string& actionName, int index)
{
    // check if the action name exists
    if (data.contains(actionName) && index >= 0 && index < data[actionName].size()) 
    {
        // remove keycode at index of action array
        data[actionName].erase(data[actionName].begin() + index);
    }
    else 
    {
        std::ostringstream oss;
        oss << "Invalid action or index.\n";
        Debug::Log(oss);
    }

    std::ofstream out("../game_config/keymaps.json");
    out << data.dump(4);
}


void N::Input::MouseCallback(GLFWwindow* window, double xpos, double ypos)
{
    mousePos = glm::dvec2(xpos, ypos);
}

void N::Input::KeyCallback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
    ProcessKeyEvent(scancode, action);
}
