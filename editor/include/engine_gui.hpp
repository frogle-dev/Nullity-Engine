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
		
		Framebuffer framebuffer;
		
		State(Nullity::State& engState);
	};

	void Styling(float* _accent, float* _accent2, float* _bg1, float* _bg2);
	void KeybindChangePopup();
	void InfoWindow(float msPerFrame, int fps);
	void InspectorWindow(entt::registry& registry);
	void DebugOutputWindow();

	void UpdateEditor(State& editorState, Nullity::Data& engData, Nullity::State& engState, GLFWwindow* window);
}
