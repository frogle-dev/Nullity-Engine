#pragma once

#include "engine.hpp"

#include "glad.h"
#include <GLFW/glfw3.h>

#include <entt/entt.hpp>

namespace Nullity
{
	bool init(GLFWwindow *&windowID, int width, int height);
	void ImguiInit(GLFWwindow* window);
}

