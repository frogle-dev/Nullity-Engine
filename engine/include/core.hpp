#pragma once

#include "state.hpp"
#include "framebuffer.hpp"
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
		Framebuffer framebuffer;

		Data data;
		State state;

		void EnterFrame();
		void Render(Camera& camera);
		void ExitFrame();
		void RenderFramebuffer();


	private:
		bool Init();
		void UtilityKeybinds();

		GLuint renderTexVAO;
	};
}

