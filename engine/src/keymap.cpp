
#include "glad.h"
#include <GLFW/glfw3.h>

#include "json.hpp"
#include <fstream>

#include "keymap.hpp"
#include "debugging.hpp"

#include <sstream>
#include <unordered_map>
#include <vector>



static std::unordered_map<std::string, std::vector<int>> bindings;
static std::unordered_map<std::string, ActionState> actions;
static int currentScancodePress;

void keysRefresh() 
{
    for (auto& [name, state] : actions) 
    {
        state.justPressed = false;
        state.released = false;
    }
}


bool isActionPressed(const std::string& actionName) 
{
    return actions[actionName].pressed;
}

bool isActionJustPressed(const std::string& actionName) 
{
    return actions[actionName].justPressed;
}

bool isActionReleased(const std::string& actionName) 
{
    return actions[actionName].released;
}

// called by glfw key callback thing
void processKeyEvent(int scancode, int action) 
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


static std::ifstream keymapJson("config/keymaps.json");
static nlohmann::json data = nlohmann::json::parse(keymapJson);
// returns all keymaps that have been set from "reloadConfigKeymaps()"
std::unordered_map<std::string, std::vector<int>>& getConfigKeymaps()
{
    return bindings;
}

int getCurrentScancodePressed()
{
    return currentScancodePress;
}

// reads and loads all keymaps from keymaps.json (run whenever keymaps.json is changed)
void reloadConfigKeymaps()
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

// sets an already existing mapping to another key, or creates a new one in keymaps.json. 
//bool 'addkeycode' = true, adds the keycode to the json file, as false it changes a keycode at 'index'
void setConfigKeymap(const std::string& actionName, bool addKeycode, int keycode, int index)
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

// removes an already existing mapping in keymaps.json. 
void removeConfigKeymap(const std::string& actionName, int index)
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
        Nullity::DebugLog(oss);
    }

    std::ofstream out("../game_config/keymaps.json");
    out << data.dump(4);
}
