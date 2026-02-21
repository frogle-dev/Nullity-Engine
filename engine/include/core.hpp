#pragma once

#include "state.hpp"
#include "framebuffer.hpp"
#include "camera.hpp"
#include "input.hpp"
#include "debugging.hpp"
#include "textures.hpp"
#include "time.hpp"

#include <GLFW/glfw3.h>

#include <entt/entt.hpp>


namespace Nullity
{
	class Engine
	{
	public:
		Engine();
		~Engine();

		GLFWwindow* window;
		Framebuffer framebuffer;

        entt::registry registry;

		Data data;
		State state;
		Time time;
		Input input;

		bool Running();

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

