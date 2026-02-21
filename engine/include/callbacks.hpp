#pragma once

#include "state.hpp"
#include "input.hpp"


namespace Nullity
{
	void WindowSizeCallback(GLFWwindow* window, int width, int height);
	void MouseCallback(GLFWwindow* window, double xpos, double ypos);
	void KeyCallback(GLFWwindow* window, int key, int scancode, int action, int mods);
}


