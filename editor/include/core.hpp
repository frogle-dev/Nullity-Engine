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

		void Update(GLFWwindow* window, Nullity::Engine& eng);

	private:
		void Cleanup();
	};
}
