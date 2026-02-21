#pragma once

#include "engine.hpp"

#include "imgui/imgui.h"
#include "imgui/backends/imgui_impl_glfw.h"
#include "imgui/backends/imgui_impl_opengl3.h"


namespace NullityEditor
{
	struct State
	{
		bool demoWindow = false;
		
		Nullity::Framebuffer framebuffer;
		
		State(Nullity::Engine& engine);
	};

	void Styling(float* _accent, float* _accent2, float* _bg1, float* _bg2);
	void KeybindChangePopup(Nullity::Input& input);
	void InfoWindow(float msPerFrame, int fps, Nullity::Input& input);
	void InspectorWindow(entt::registry& registry);
	void DebugOutputWindow();
}
