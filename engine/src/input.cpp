#include "glad.h"
#include <GLFW/glfw3.h>

#include "json.hpp"
#include <fstream>

#include "input.hpp"
#include "debugging.hpp"
#include "core.hpp"

#include <sstream>
#include <unordered_map>
#include <vector>


Nullity::Input::Input()
    : keymapJson("config/keymaps.json")
{
    data = nlohmann::json::parse(keymapJson);
}


void Nullity::Input::keysRefresh() 
{
    for (auto& [name, state] : actions) 
    {
        state.justPressed = false;
        state.released = false;
    }
}


bool Nullity::Input::isActionPressed(const std::string& actionName) 
{
    return actions[actionName].pressed;
}

bool Nullity::Input::isActionJustPressed(const std::string& actionName) 
{
    return actions[actionName].justPressed;
}

bool Nullity::Input::isActionReleased(const std::string& actionName) 
{
    return actions[actionName].released;
}

// called by glfw key callback thing
void Nullity::Input::processKeyEvent(int scancode, int action) 
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

std::unordered_map<std::string, std::vector<int>>& Nullity::Input::getConfigKeymaps()
{
    return bindings;
}

int Nullity::Input::getCurrentScancodePressed()
{
    return currentScancodePress;
}

void Nullity::Input::reloadConfigKeymaps()
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

void Nullity::Input::setConfigKeymap(const std::string& actionName, bool addKeycode, int keycode, int index)
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

void Nullity::Input::removeConfigKeymap(const std::string& actionName, int index)
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
        debug.Log(oss);
    }

    std::ofstream out("../game_config/keymaps.json");
    out << data.dump(4);
}


void Nullity::MouseCallback(GLFWwindow* window, double xpos, double ypos)
{
    Engine* eng = static_cast<Engine*>(glfwGetWindowUserPointer(window));

    eng->input.mousePos = glm::dvec2(xpos, ypos);
}

void Nullity::KeyCallback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
    Engine* eng = static_cast<Engine*>(glfwGetWindowUserPointer(window));

    eng->input.processKeyEvent(scancode, action);
}
