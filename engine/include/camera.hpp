#pragma once

#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"

namespace Nullity
{
    const float def_yaw = -90.0f;
    const float def_pitch = 0.0f;
    const float def_sensitivity = 0.1f;
    const float def_fov = 85.0f;

    class Camera
    {
    public:
        glm::vec3 position = glm::vec3(0);
        glm::vec3 front = glm::vec3(0);
        glm::vec3 straightFront = glm::vec3(0); // follows yaw movement but not pitch, used for purely horizontal movement
        glm::vec3 up = glm::vec3(0);
        glm::vec3 right = glm::vec3(0);
        glm::vec3 worldUp = glm::vec3(0);

        float yaw = 0;
        float pitch = 0;

        float sensitivity = 0;
        float fov = 0;

        Camera(glm::vec3 _position = glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3 _up = glm::vec3(0.0f, 1.0f, 0.0f), float _yaw = def_yaw, float _pitch = def_pitch);
        glm::mat4 GetViewMatrix();
        void ProcessMouseMovement(float xOffset, float yOffset);

    private:
        void UpdateCameraVectors();
    };
}
