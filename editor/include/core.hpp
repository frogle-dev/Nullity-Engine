#pragma once

#include "engine.hpp"

namespace NullityEditor
{
	void EditorInit();
	void EditorExit();

	inline bool demoWindow = false;
	inline Nullity::Framebuffer framebuffer;

	void EnterFrame();
	void Update();
	void ExitFrame();

	void UtilityKeybinds();
	void Cleanup();
}
