#pragma once

#include "glad.h"
#include <GLFW/glfw3.h>

#include "engine.hpp"
#include "framebuffer.hpp"

#include <entt/entt.hpp>

namespace Nullity
{
	struct GUIData
	{
		Framebuffer frameBuffer;

		GUIData(State& engState);
	};

	void Styling(float* _accent, float* _accent2, float* _bg1, float* _bg2);
	void KeybindChangePopup();
	void InfoWindow(float msPerFrame, int fps);
	void InspectorWindow(entt::registry& registry);
	void DebugOutputWindow();

	void UpdateEngineGUI(State& eng, GUIData& guiData, Data& engData, GLFWwindow* window);
}
