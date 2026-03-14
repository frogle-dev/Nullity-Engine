#pragma once

#include "systems.hpp"
#include <entt/entt.hpp>

namespace Nullity
{
	class sys_Render final : System
	{
	public:
		void Update() override;
	};

	void WorldObjectSystem();
}
