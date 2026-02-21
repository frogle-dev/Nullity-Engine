#include <entt/entt.hpp>

namespace Nullity
{
	class Entity
	{
	public:
		Entity(entt::registry& registry);

		template<typename Type, typename... Parameters>
		Entity& Add(Parameters&&... params)
		{
			registry.emplace<Type>(id, std::forward<Parameters>(params)...);
			return *this;
		}

		entt::entity GetID() { return id; }

	private:
		entt::registry& registry;
		entt::entity id;
	};
}

