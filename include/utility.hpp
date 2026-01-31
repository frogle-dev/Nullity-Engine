#pragma once

#include "glad.h"
#include <GLFW/glfw3.h>

#include <entt/entt.hpp>

#include "keymap.hpp"


struct EngineState
{
    bool focus = true;
    bool wireframe = false;
};

void UtilityKeybinds(GLFWwindow* window, EngineState& engineState);