#pragma once

#include "engine.hpp"

#include "glad.h"
#include <GLFW/glfw3.h>

#include <entt/entt.hpp>

namespace Nullity
{
	bool Init(GLFWwindow* windowID, State& engState);
	void ImguiInit(GLFWwindow* window);
}

