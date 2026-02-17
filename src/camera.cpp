#include "camera.hpp"

#include "glad.h"
#include <GLFW/glfw3.h>
#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"

#include <algorithm>

#include "engine.hpp"


Camera::Camera(glm::vec3 _position, glm::vec3 _up, float _yaw, float _pitch)
{
    front = glm::vec3(0.0f, 0.0f, -1.0f);
    sensitivity = def_sensitivity;
    fov = def_fov;

    position = _position;
    worldUp = _up;
    yaw = _yaw;
    pitch = _pitch;
    UpdateCameraVectors();
}

glm::mat4 Camera::GetViewMatrix()
{
    return glm::lookAt(
        position,
        position + front,
        up
    );
}

void Camera::ProcessMouseMovement(float xOffset, float yOffset)
{
    xOffset *= sensitivity;
    yOffset *= sensitivity;

    yaw += xOffset;
    pitch += yOffset;

    pitch = std::clamp(pitch, -89.0f, 89.0f);

    UpdateCameraVectors();
}

void Camera::UpdateCameraVectors()
{
    glm::vec3 direction;
    direction.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
    direction.y = sin(glm::radians(pitch));
    direction.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
    front = glm::normalize(direction);

    right = glm::normalize(glm::cross(front, worldUp));
    up = glm::normalize(glm::cross(right, front));

    straightFront = glm::normalize(glm::cross(right, worldUp));
}

void CameraControls(Nullity::MouseState& mouse, Nullity::State& engineState, Camera& camera)
{
    if (mouse.firstMouse)
    {
        mouse.lastMousePos = mouse.mousePos;
        mouse.firstMouse = false;
    } // this is so when mouse initially moves, it doesnt make a large jkittery motion to that position

    if (engineState.focus)
    {
        float xOffset = mouse.mousePos.x - mouse.lastMousePos.x;
        float yOffset = mouse.lastMousePos.y - mouse.mousePos.y;
        mouse.lastMousePos = mouse.mousePos;
    
        camera.ProcessMouseMovement(xOffset, yOffset);
    }
}
