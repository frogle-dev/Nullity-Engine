#pragma once

#include "engine_gui.hpp"


namespace NullityEditor
{
	class Editor
	{
	public:
		Editor(GLFWwindow* window, Nullity::Engine& engine);
		~Editor();

		State state;
	
	private:
		void Cleanup();
	};
}
