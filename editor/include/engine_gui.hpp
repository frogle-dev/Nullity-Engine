#pragma once

#include "engine.hpp"

#include "imgui/imgui.h"
#include "imgui/backends/imgui_impl_glfw.h"
#include "imgui/backends/imgui_impl_opengl3.h"
#include "include/core.hpp"


namespace NullityEditor
{
	struct State
	{
		bool demoWindow = false;
		
		Framebuffer framebuffer;
		
		State(Nullity::Engine& engine);
	};

    void UtilityKeybinds(Nullity::Engine& eng);

	void Styling(float* _accent, float* _accent2, float* _bg1, float* _bg2);
	void KeybindChangePopup();
	void InfoWindow(float msPerFrame, int fps);
	void InspectorWindow(entt::registry& registry);
	void DebugOutputWindow();
}
