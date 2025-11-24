#pragma once

#include <GLFW/glfw3.h>

#include "../lib/json.hpp"
#include <fstream>

#include <unordered_map>
#include <vector>
#include <iostream>


struct ActionState 
{
    bool pressed = false;
    bool justPressed = false;
    bool released = false;
};

std::unordered_map<std::string, std::vector<int>> bindings;
std::unordered_map<std::string, ActionState> actions;
int currentKeyPress;

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
void processKeyEvent(int key, int action) 
{
    currentKeyPress = key;

    for (auto& [actionName, keys] : bindings) 
    {
        for (int boundKey : keys)
        {
            if (boundKey == key) 
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


std::ifstream keymapJson("../game_config/keymaps.json");
nlohmann::json data = nlohmann::json::parse(keymapJson);
// returns all keymaps that have been set from "reloadConfigKeymaps()"
const auto& getConfigKeymaps()
{
    return bindings;
}

// reads and loads all keymaps from keymaps.json (run whenever keymaps.json is changed)
void reloadConfigKeymaps()
{
    for (auto& [actionName, keycode] : data.items())
    {
        bindings[actionName].push_back(keycode);
    }
}

// sets an already existing mapping to another key, or creates a new one in keymaps.json
void setConfigKeymap(const std::string& actionName, int keycode)
{
    data[actionName] = keycode;

    std::ofstream out("../game_config/keymaps.json");
    out << data.dump(4);
}