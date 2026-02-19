#pragma once

#include "state.hpp"

#include "camera.hpp"

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

		void EnterFrame();
		void Render(Camera& camera);
		void ExitFrame();

	private:
		bool Init();
	};
}

