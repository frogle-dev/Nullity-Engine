#include "render.hpp"
#include "components.hpp"

#include <entt/entt.hpp>


void DrawSystem(entt::registry& registry)
{
    auto view = registry.view<ObjectModel, ObjectShader>();

    for (auto [entity, cmp_model, cmp_shader] : view.each())
    {
        if (cmp_model.render)
        {
            cmp_model.model.Draw(cmp_shader.shader);
        }
    }
}
