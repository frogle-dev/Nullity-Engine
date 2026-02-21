#include "callbacks.hpp"
#include "core.hpp"

#include <iostream>


void Nullity::WindowSizeCallback(GLFWwindow* window, int width, int height)
{
    Engine* eng = static_cast<Engine*>(glfwGetWindowUserPointer(window));

    State* state = &eng->state;

    // letterbox scaling
    float aspect = (float)width / height;
    float targetAspect = (float)state->initViewRes.x / (float)state->initViewRes.y;

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

    state->viewOffset.x = (width - state->viewRes.x) / 2;
    state->viewOffset.y = (height - state->viewRes.y) / 2;

    glViewport(state->viewOffset.x, state->viewOffset.y, state->viewRes.x, state->viewRes.y);

    eng->framebuffer.Refresh(state->viewRes.x, state->viewRes.y);
}
