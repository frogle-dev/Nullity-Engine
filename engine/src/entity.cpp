#include "entity.hpp"

using namespace Nullity;


Entity::Entity(entt::registry& registry)
	: registry(registry), id(registry.create()) {}
