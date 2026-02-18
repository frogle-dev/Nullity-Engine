#pragma once

#include "state.hpp"

#include <GLFW/glfw3.h>


namespace Nullity
{
	class Engine
	{
	public:
		Engine(GLFWwindow* window);
		~Engine();

		Data data;
		State state;

		void Update();

	private:

	};
}

