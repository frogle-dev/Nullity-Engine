#pragma once

#include "engine_gui.hpp"


namespace NullityEditor
{
	class Editor
	{
	public:
		Editor(Nullity::Engine& engine);
		~Editor();

		State state;

		void EnterFrame();
		void Update(Nullity::Engine& eng);
		void ExitFrame();

	private:
		void UtilityKeybinds(Nullity::Engine& eng);
		void Cleanup();
	};
}
