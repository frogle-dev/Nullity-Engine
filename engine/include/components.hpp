#pragma once

#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/type_ptr.hpp"

#include "models.hpp"
#include "shader.hpp"

#include <string>


namespace Nullity::Components
{
    struct DisplayName
    {
        std::string name;
    };

    struct WorldObject
    {
        glm::mat4 modelMatrix = glm::mat4(1.0f);
    };

    struct ObjectShader
    {
        Shader& shader;
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
}
