#include "engine.hpp"

#include "glad.h"
#include <GLFW/glfw3.h>

#include <entt/entt.hpp>

#include "components.hpp"
#include "keymap.hpp"
#include "shader.hpp"
#include "primitives.hpp"
#include "textures.hpp"


void Nullity::UtilityKeybinds(GLFWwindow* window, State& engineState)
{
    if (isActionJustPressed("focus"))
    {
        engineState.focus = !engineState.focus;

        if (engineState.focus)
            glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
        else
            glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
    }

    if (isActionJustPressed("wireframe"))
    {
        engineState.wireframe = !engineState.wireframe;

        if (engineState.wireframe)
            glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
        else
            glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    }
}

Nullity::Data::Data()
{
    InitSkybox();
    InitUBOs();
}

void Nullity::Data::InitUBOs()
{
    glGenBuffers(1, &matricesUBO);
    glBindBuffer(GL_UNIFORM_BUFFER, matricesUBO);
    glBufferData(GL_UNIFORM_BUFFER, 128, NULL, GL_STATIC_DRAW);
    glBindBufferBase(GL_UNIFORM_BUFFER, 0, matricesUBO);
    glBindBuffer(GL_UNIFORM_BUFFER, 0);
    
    glGenBuffers(1, &texArrayDataUBO);
    glBindBuffer(GL_UNIFORM_BUFFER, texArrayDataUBO);
    glBufferData(GL_UNIFORM_BUFFER, 1616, NULL, GL_STATIC_DRAW);
    glBindBufferBase(GL_UNIFORM_BUFFER, 1, texArrayDataUBO);
    glBindBuffer(GL_UNIFORM_BUFFER, 0);
}

void Nullity::Data::InitSkybox()
{
    glGenVertexArrays(1, &skyboxVAO);
    glGenBuffers(1, &skyboxVBO);
    glBindVertexArray(skyboxVAO);
    glBindBuffer(GL_ARRAY_BUFFER, skyboxVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(skyboxVertices), &skyboxVertices, GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);

    std::vector<std::string> skyboxFaces = {
        "images/skybox/right.jpg",
        "images/skybox/left.jpg",
        "images/skybox/top.jpg",
        "images/skybox/bottom.jpg",
        "images/skybox/front.jpg",
        "images/skybox/back.jpg",
    };
    skyboxCubemap = TextureManager::Get().LoadCubemap(skyboxFaces);
}

void Nullity::Data::Cleanup()
{
    glDeleteBuffers(1, &matricesUBO);
    glDeleteBuffers(1, &texArrayDataUBO);
    glDeleteVertexArrays(1, &skyboxVAO);
    glDeleteBuffers(1, &skyboxVBO);
    
    objectShader.deleteProgram();
    lightSourceShader.deleteProgram();
    skyboxShader.deleteProgram();
    instancedShader.deleteProgram();
    grassShader.deleteProgram();
    unlitShader.deleteProgram();
}
