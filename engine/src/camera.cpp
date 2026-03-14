#include "camera.hpp"
#include "engine.hpp"

#include "glad.h"
#include <GLFW/glfw3.h>

#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/type_ptr.hpp"

namespace N = Nullity;

void N::sys_Camera::Update()
{
    auto view = N::registry.view<Camera, Components::Transform>();

    for (auto [entity, cam, t] : view.each())
    {
        glm::vec3 direction;
        direction.x = cos(glm::radians(t.rotation.y)) * cos(glm::radians(t.rotation.x));
        direction.y = sin(glm::radians(t.rotation.x));
        direction.z = sin(glm::radians(t.rotation.y)) * cos(glm::radians(t.rotation.x));
        cam.front = glm::normalize(direction);

        cam.right = glm::normalize(glm::cross(cam.front, cam.worldUp));
        cam.up = glm::normalize(glm::cross(cam.right, cam.front));

        cam.straightFront = glm::normalize(glm::cross(cam.right, cam.worldUp));
    }
}

glm::mat4 N::GetCameraViewMatrix(const Camera& cam, const Components::Transform& t)
{
    return glm::lookAt(
        t.position,
        t.position + cam.front,
        cam.up
    );
}

// N::Camera::Camera(glm::vec3 _position, glm::vec3 _up, float _yaw, float _pitch)
// {
//     front = glm::vec3(0.0f, 0.0f, -1.0f);
//     sensitivity = def_sensitivity;
//     fov = def_fov;
//
//     position = _position;
//     worldUp = _up;
//     yaw = _yaw;
//     pitch = _pitch;
//     UpdateCameraVectors();
// }





