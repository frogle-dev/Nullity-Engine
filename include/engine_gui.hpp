#pragma once

#include "imgui/imgui.h"
#include "imgui/backends/imgui_impl_glfw.h"
#include "imgui/backends/imgui_impl_opengl3.h"

#include "keymap.hpp"

#include <entt/entt.hpp>

namespace Nullity
{
	void Styling(float* _accent, float* _accent2, float* _bg1, float* _bg2);
	void KeybindChangePopup();
	void InfoWindow(float msPerFrame, int fps);
	void InspectorWindow(entt::registry& registry);
	void DebugOutputWindow();
}
