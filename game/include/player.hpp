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

struct Camera_Controller
{
    float sensitivity = 0.1f;
};

void PlayerUpdate();
void CameraControls();
