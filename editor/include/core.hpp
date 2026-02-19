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

		void Update(Nullity::Engine& eng);

	private:
		void Cleanup();
	};
}
