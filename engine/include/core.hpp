#pragma once

#include "state.hpp"


namespace Nullity
{
	class Engine
	{
	public:
		Engine(GLFWwindow* window);
		~Engine();

		Data data;
		State state;

	private:

	};
}

