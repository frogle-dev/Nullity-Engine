#include "render.hpp"
#include "camera.hpp"
#include "core.hpp"
#include "textures.hpp"
#include "components.hpp"

#include <entt/entt.hpp>

#include <iostream>
namespace N = Nullity;
using namespace Nullity::Components;

void DrawModels()
{
    auto view = N::registry.view<ObjectModel, ObjectShader>();

    for (auto [entity, cmp_model, cmp_shader] : view.each())
    {
        if (cmp_model.render)
        {
            cmp_model.model.Draw(cmp_shader.shader);
        }
    }
}

void N::WorldObjectSystem()
{
    auto view = Nullity::registry.view<WorldObject, Transform, ObjectShader>();

    for (auto [entity, cmp_object, cmp_transform, cmp_shader] : view.each())
    {
        cmp_shader.shader.use();

        cmp_object.modelMatrix = glm::mat4(1.0f);
        cmp_object.modelMatrix = glm::translate(cmp_object.modelMatrix, cmp_transform.position);
        cmp_object.modelMatrix = glm::rotate(cmp_object.modelMatrix, glm::radians(cmp_transform.rotation.x), glm::vec3(1.0f, 0.0f, 0.0f));
        cmp_object.modelMatrix = glm::rotate(cmp_object.modelMatrix, glm::radians(cmp_transform.rotation.y), glm::vec3(0.0f, 1.0f, 0.0f));
        cmp_object.modelMatrix = glm::rotate(cmp_object.modelMatrix, glm::radians(cmp_transform.rotation.z), glm::vec3(0.0f, 0.0f, 1.0f));
        cmp_object.modelMatrix = glm::scale(cmp_object.modelMatrix, cmp_transform.scale);

        cmp_shader.shader.setMat4("model", cmp_object.modelMatrix);
    }
}

void N::sys_Render::Update()
{
    auto view = registry.view<Camera, Transform>();

    for (auto [entity, cam, cam_tform] : view.each())
    {
        Data::objectShader.use();
        Data::objectShader.setVec3("viewPos", cam_tform.position);

        Textures::SendSubTexResArrayToShader(Data::texArrayDataUBO);

        glm::mat4 view = GetCameraViewMatrix(cam, cam_tform);
        glm::mat4 projection = glm::perspective(glm::radians(cam.fov), (float)viewRes.x / viewRes.y, 0.1f, 1000.0f);

        SetUniformBufferData(Data::matricesUBO, 0, 64, glm::value_ptr(view));
        SetUniformBufferData(Data::matricesUBO, 64, 64, glm::value_ptr(projection));
        
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_CUBE_MAP, Data::skyboxCubemap); // binding skybox for reflections

        WorldObjectSystem();
        DrawModels();

        // skybox
        glDepthFunc(GL_LEQUAL);
        Data::skyboxShader.use();
        view = glm::mat4(glm::mat3(GetCameraViewMatrix(cam, cam_tform)));

        SetUniformBufferData(Data::matricesUBO, 0, 64, glm::value_ptr(view));

        glBindVertexArray(Data::skyboxVAO);
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_CUBE_MAP, Data::skyboxCubemap);
        glDrawArrays(GL_TRIANGLES, 0, 36);
        glDepthFunc(GL_LESS);

        glBindVertexArray(0);

        framebuffer.Unbind();
    }
}


