#pragma once

#include "state.hpp"

#include <GLFW/glfw3.h>


namespace Nullity
{
	class Engine
	{
	public:
		Engine();
		~Engine();

		GLFWwindow* window;

		Data data;
		State state;

		void Update();

	private:
		bool Init();
	};
}

