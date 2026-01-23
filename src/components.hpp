#pragma once

#include "../lib/glad.h"
#include <GLFW/glfw3.h>
#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/type_ptr.hpp"

#include "models.hpp"
#include "shader.hpp"

#include <entt/entt.hpp>


struct WorldObject
{
    Shader& shader;

    glm::mat4 modelMatrix = glm::mat4(1.0f);
};

struct ObjectModel
{
    Model model;
    bool render;
};

struct Transform
{
    glm::vec3 position = glm::vec3(0.0f);
    glm::vec3 rotation = glm::vec3(0.0f);
    glm::vec3 scale = glm::vec3(1.0f);
};

struct Velocity
{
    glm::vec3 velocity = glm::vec3(0.0f);
};

struct Player
{
    float bodyHeight = 2.0f;
    float gravity = -9.81f * 2.0f; 
    bool grounded = false;
    float speed = 8.0f; 
    float jumpForce = 7.0f;
    glm::vec3 moveDir;
};