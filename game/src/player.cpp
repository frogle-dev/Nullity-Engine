#include "engine.hpp"


void PlayerUpdate(entt::registry& registry, Camera& camera, float deltaTime)
{
    auto view = registry.view<Transform, Velocity, Player>();

    for (auto [entity, cmp_transform, cmp_velocity, cmp_player] : view.each())
    {
        cmp_player.moveDir = glm::vec3(0.0f);
        cmp_velocity.velocity = glm::vec3(0.0f, cmp_velocity.velocity.y, 0.0f);

        if (isActionPressed("forward"))
        {
            cmp_player.moveDir -= camera.straightFront;
        }
        if (isActionPressed("backward"))
        {
            cmp_player.moveDir += camera.straightFront;
        }
        if (isActionPressed("left"))
        {
            cmp_player.moveDir -= camera.right;
        }
        if (isActionPressed("right"))
        {
            cmp_player.moveDir += camera.right;
        }

        if (glm::length(cmp_player.moveDir) > 0.0f)
        {
            cmp_player.moveDir = glm::normalize(cmp_player.moveDir);
            cmp_velocity.velocity.x = cmp_player.moveDir.x * cmp_player.speed; 
            cmp_velocity.velocity.z = cmp_player.moveDir.z * cmp_player.speed;
        }

        if (isActionPressed("jump") && cmp_player.grounded)
        {
            cmp_player.grounded = false;
            cmp_velocity.velocity.y = cmp_player.jumpForce;
        }

        cmp_velocity.velocity.y += cmp_player.gravity * deltaTime;
        cmp_transform.position += cmp_velocity.velocity * deltaTime;

        if (cmp_transform.position.y <= 1.0f)
        {
            cmp_player.grounded = true;
            cmp_transform.position.y = 1.0f;
            cmp_velocity.velocity.y = 0.0f;
        }
        else
        {
            cmp_player.grounded = false;
        }

        camera.position = glm::vec3(cmp_transform.position.x, cmp_transform.position.y + cmp_player.bodyHeight, cmp_transform.position.z);

        keysRefresh();
    }
}
