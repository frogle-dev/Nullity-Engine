#include "player.hpp"

namespace N = Nullity;
using namespace Nullity::Components;


void PlayerUpdate()
{
    auto view = N::registry.view<Transform, Velocity, Player, N::Camera, Transform>();

    for (auto [entity, cmp_transform, cmp_velocity, cmp_player, cam, cam_tform] : view.each())
    {
        cmp_player.moveDir = glm::vec3(0.0f);
        cmp_velocity.velocity = glm::vec3(0.0f, cmp_velocity.velocity.y, 0.0f);

        if (N::Input::IsActionPressed("forward"))
        {
            cmp_player.moveDir -= cam.straightFront;
        }
        if (N::Input::IsActionPressed("backward"))
        {
            cmp_player.moveDir += cam.straightFront;
        }
        if (N::Input::IsActionPressed("left"))
        {
            cmp_player.moveDir -= cam.right;
        }
        if (N::Input::IsActionPressed("right"))
        {
            cmp_player.moveDir += cam.right;
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

        cam_tform.position = glm::vec3(cmp_transform.position.x, cmp_transform.position.y + cmp_player.bodyHeight, cmp_transform.position.z);
    }
}

void ProcessCameraMovement(float xOffset, float yOffset, Camera_Controller& cam_control, Transform& cam_tform)
{
    xOffset *= cam_control.sensitivity;
    yOffset *= cam_control.sensitivity;

    cam_tform.rotation.y += xOffset;
    cam_tform.rotation.x += yOffset;

    cam_tform.rotation.x = std::clamp(cam_tform.rotation.x, -89.0f, 89.0f);
}

void CameraControls()
{
    auto view = N::registry.view<Transform, Camera_Controller, N::Camera>();
    
    for (auto [entity, cam_tform, cam_control, cam] : view.each())
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

            ProcessCameraMovement(xOffset, yOffset, cam_control, cam_tform);
        }
    }
}
