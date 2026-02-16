#pragma once

#include "glad.h"
#include <GLFW/glfw3.h>

#include <entt/entt.hpp>

namespace Engine
{
	bool init(GLFWwindow *&windowID, int width, int height);
	void ImguiInit(GLFWwindow* window);
}

