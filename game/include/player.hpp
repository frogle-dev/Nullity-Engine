#pragma once

#include "engine.hpp"

struct Player
{
    float bodyHeight = 2.0f;
    float gravity = -9.81f * 2.0f; 
    bool grounded = false;
    float speed = 8.0f; 
    float jumpForce = 7.0f;
    glm::vec3 moveDir;
};

void PlayerUpdate(entt::registry& registry, Nullity::Camera& camera, float deltaTime, Nullity::Input& input);
void CameraControls(Nullity::MouseState& mouse, Nullity::State& engineState, Nullity::Camera& camera);
