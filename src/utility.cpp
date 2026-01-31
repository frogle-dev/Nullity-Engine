#include "utility.hpp"

#include "glad.h"
#include <GLFW/glfw3.h>

#include <entt/entt.hpp>

#include "keymap.hpp"

#include <sstream>

void UtilityKeybinds(GLFWwindow* window, EngineState& engineState)
{
    if (isActionJustPressed("focus"))
    {
        engineState.focus = !engineState.focus;

        if (engineState.focus)
            glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
        else
            glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
    }

    if (isActionJustPressed("wireframe"))
    {
        engineState.wireframe = !engineState.wireframe;

        if (engineState.wireframe)
            glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
        else
            glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    }
}