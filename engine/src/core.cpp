#include "core.hpp"
#include "state.hpp"
#include "init.hpp"
#include "debugging.hpp"

#include <GLFW/glfw3.h>


Nullity::Engine::Engine(GLFWwindow* window)
{
	if(!Init(window))
	{
        std::ostringstream oss;
        oss << "Engine failed to initialize" << std::endl;
        Nullity::DebugLog(oss);
	}
}

Nullity::Engine::~Engine()
{
	data.Cleanup();

    glfwTerminate();
}


