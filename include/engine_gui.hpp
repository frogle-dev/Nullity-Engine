#pragma once

#include "imgui/imgui.h"
#include "imgui/backends/imgui_impl_glfw.h"
#include "imgui/backends/imgui_impl_opengl3.h"

#include "keymap.hpp"

#include <entt/entt.hpp>


extern std::string currentActionName;
extern std::vector<int> currentKeycodes;
extern bool changingKeybind;

void KeybindChangePopup();
void InfoWindow(float msPerFrame, int fps);
