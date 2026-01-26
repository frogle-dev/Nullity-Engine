#include "render.hpp"

#include "components.hpp"

#include <entt/entt.hpp>

void DrawSystem(entt::registry& registry)
{
    auto view = registry.view<ObjectModel, WorldObject>();

    for (auto [entity, cmp_model, cmp_object] : view.each())
    {
        if (cmp_model.render)
        {
            cmp_model.model.Draw(cmp_object.shader);
        }
    }
}