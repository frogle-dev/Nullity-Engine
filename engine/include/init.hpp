#pragma once

#include "glad.h"
#include <GLFW/glfw3.h>

#include "state.hpp"


namespace Nullity
{
	bool Init(GLFWwindow *&windowID, State& engState);
}

