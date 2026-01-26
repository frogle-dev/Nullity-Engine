#pragma once

#include "debugging.hpp"

#include <GLFW/glfw3.h>

#include "json.hpp"
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

extern std::unordered_map<std::string, std::vector<int>> bindings;
extern std::unordered_map<std::string, ActionState> actions;
extern int currentScancodePress;

void keysRefresh(); 

bool isActionPressed(const std::string& actionName); 

bool isActionJustPressed(const std::string& actionName);

bool isActionReleased(const std::string& actionName);

// called by glfw key callback thing
void processKeyEvent(int scancode, int action);


extern std::ifstream keymapJson;
extern nlohmann::json data;
// returns all keymaps that have been set from "reloadConfigKeymaps()"
std::unordered_map<std::string, std::vector<int>>& getConfigKeymaps();

// reads and loads all keymaps from keymaps.json (run whenever keymaps.json is changed)
void reloadConfigKeymaps();

// sets an already existing mapping to another key, or creates a new one in keymaps.json. 
//bool 'addkeycode' = true, adds the keycode to the json file, as false it changes a keycode at 'index'
void setConfigKeymap(const std::string& actionName, bool addKeycode, int keycode, int index = 0);

// removes an already existing mapping in keymaps.json. 
void removeConfigKeymap(const std::string& actionName, int index);
