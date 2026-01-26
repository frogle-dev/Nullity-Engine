#include "utility.hpp"

#include "glad.h"
#include <GLFW/glfw3.h>

#include <entt/entt.hpp>

#include "keymap.hpp"


bool focus = true;
bool wireframe = false;
void UtilityKeybinds(GLFWwindow* window)
{
    if (isActionJustPressed("focus"))
    {
        focus = !focus;

        if (focus)
            glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
        else
            glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
    }

    if (isActionJustPressed("wireframe"))
    {
        wireframe = !wireframe;

        if (wireframe)
            glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
        else
            glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    }
}