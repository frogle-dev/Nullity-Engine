#include "player.hpp"

namespace N = Nullity;
using namespace Nullity::Components;


void PlayerUpdate(entt::registry& registry, Nullity::Camera& camera)
{
    auto view = registry.view<Transform, Velocity, Player>();

    for (auto [entity, cmp_transform, cmp_velocity, cmp_player] : view.each())
    {
        cmp_player.moveDir = glm::vec3(0.0f);
        cmp_velocity.velocity = glm::vec3(0.0f, cmp_velocity.velocity.y, 0.0f);

        if (N::Input::IsActionPressed("forward"))
        {
            cmp_player.moveDir -= camera.straightFront;
        }
        if (N::Input::IsActionPressed("backward"))
        {
            cmp_player.moveDir += camera.straightFront;
        }
        if (N::Input::IsActionPressed("left"))
        {
            cmp_player.moveDir -= camera.right;
        }
        if (N::Input::IsActionPressed("right"))
        {
            cmp_player.moveDir += camera.right;
        }

        if (glm::length(cmp_player.moveDir) > 0.0f)
        {
            cmp_player.moveDir = glm::normalize(cmp_player.moveDir);
            cmp_velocity.velocity.x = cmp_player.moveDir.x * cmp_player.speed; 
            cmp_velocity.velocity.z = cmp_player.moveDir.z * cmp_player.speed;
        }

        if (N::Input::IsActionPressed("jump") && cmp_player.grounded)
        {
            cmp_player.grounded = false;
            cmp_velocity.velocity.y = cmp_player.jumpForce;
        }

        cmp_velocity.velocity.y += cmp_player.gravity * N::Time::deltaTime;
        cmp_transform.position += cmp_velocity.velocity * N::Time::deltaTime;

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
    }
}

void CameraControls(Nullity::Camera& camera)
{
    if (N::Input::firstMouse)
    {
        N::Input::lastMousePos = N::Input::mousePos;
        N::Input::firstMouse = false;
    } // this is so when mouse initially moves, it doesnt make a large jkittery motion to that position

    if (N::focused)
    {
        float xOffset = N::Input::mousePos.x - N::Input::lastMousePos.x;
        float yOffset = N::Input::lastMousePos.y - N::Input::mousePos.y;
        N::Input::lastMousePos = N::Input::mousePos;

        camera.ProcessMouseMovement(xOffset, yOffset);
    }
}
