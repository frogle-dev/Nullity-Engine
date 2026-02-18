#include "callbacks.hpp"


void Nullity::WindowSizeCallback(GLFWwindow* window, int width, int height)
{
    State* state = static_cast<State*>(glfwGetWindowUserPointer(window));

    // letterbox scaling
    float aspect = (float)width / height;
    float targetAspect = (float)state->initViewRes.x / state->initViewRes.y;

    if (aspect > targetAspect)
    {
        state->viewRes.y = height;
        state->viewRes.x = (int)(height * targetAspect);
    }
    else
    {
        state->viewRes.x = width;
        state->viewRes.y = (int)(width / targetAspect);
    }

    int viewX = (width - state->viewRes.x) / 2;
    int viewY = (height - state->viewRes.y) / 2;

    glViewport(viewX, viewY, state->viewRes.x, state->viewRes.y);
}


void Nullity::MouseCallback(GLFWwindow* window, double xpos, double ypos)
{
    State* state = static_cast<State*>(glfwGetWindowUserPointer(window));

    state->mouse->mousePos = glm::dvec2(xpos, ypos);
}

void Nullity::KeyCallback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
    processKeyEvent(scancode, action);
}
