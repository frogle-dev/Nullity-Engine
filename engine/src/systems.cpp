#include "glad.h"
#include <GLFW/glfw3.h>
#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/type_ptr.hpp"

#include "systems.hpp"
#include "components.hpp"
#include "shader.hpp"

#include <entt/entt.hpp>


void WorldObjectSystem(entt::registry& registry)
{
    auto view = registry.view<WorldObject, Transform, ObjectShader>();

    for (auto [entity, cmp_object, cmp_transform, cmp_shader] : view.each())
    {
        cmp_shader.shader.use();
        
        cmp_object.modelMatrix = glm::mat4(1.0f);
        cmp_object.modelMatrix = glm::translate(cmp_object.modelMatrix, cmp_transform.position);
        cmp_object.modelMatrix = glm::rotate(cmp_object.modelMatrix, glm::radians(cmp_transform.rotation.x), glm::vec3(1.0f, 0.0f, 0.0f));
        cmp_object.modelMatrix = glm::rotate(cmp_object.modelMatrix, glm::radians(cmp_transform.rotation.y), glm::vec3(0.0f, 1.0f, 0.0f));
        cmp_object.modelMatrix = glm::rotate(cmp_object.modelMatrix, glm::radians(cmp_transform.rotation.z), glm::vec3(0.0f, 0.0f, 1.0f));
        cmp_object.modelMatrix = glm::scale(cmp_object.modelMatrix, cmp_transform.scale);

        cmp_shader.shader.setMat4("model", cmp_object.modelMatrix);
    }
}
