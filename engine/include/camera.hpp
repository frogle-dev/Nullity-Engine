#pragma once

#include "components.hpp"
#include "systems.hpp"
#include "glm/glm.hpp"

namespace Nullity
{
    class sys_Camera final : System
    {
    public:
        void Update() override;
    };

    struct Camera
    {
        // glm::vec3 position = glm::vec3(0);
        // float yaw = -90.0f;
        // float pitch = 0.0f;

        glm::vec3 up = glm::vec3(0);
        glm::vec3 front = glm::vec3(0);
        glm::vec3 straightFront = glm::vec3(0); // follows yaw movement but not pitch, used for purely horizontal movement
        glm::vec3 right = glm::vec3(0);
        glm::vec3 worldUp = glm::vec3(0.0f, 1.0f, 0.0f);

        float fov = 85.0f;

        // Camera(glm::vec3 _position = glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3 _up = glm::vec3(0.0f, 1.0f, 0.0f), float _yaw = def_yaw, float _pitch = def_pitch);
    };

    glm::mat4 GetCameraViewMatrix(const Camera& cam, const Components::Transform& transform);
}
