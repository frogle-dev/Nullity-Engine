#include "glad.h"
#include <GLFW/glfw3.h>

#include <entt/entt.hpp>

#include "core.hpp"
#include "state.hpp"
#include "input.hpp"
#include "shader.hpp"
#include "primitives.hpp"
#include "textures.hpp"


void Nullity::Data::InitData()
{
    InitShaders();
    InitSkybox();
    InitUBOs();
}

void Nullity::Data::InitShaders()
{
    objectShader = Shader("assets/shaders/lit.vert", "assets/shaders/lit.frag");
    lightSourceShader = Shader("assets/shaders/light_source.vert", "assets/shaders/light_source.frag");
    skyboxShader = Shader("assets/shaders/skybox.vert", "assets/shaders/skybox.frag");
    instancedShader = Shader("assets/shaders/instanced.vert", "assets/shaders/instanced.frag"); 
    grassShader = Shader("assets/shaders/grass.vert", "assets/shaders/grass.frag");
    unlitShader = Shader("assets/shaders/unlit.vert", "assets/shaders/unlit.frag");
    renderTexShader = Shader("assets/shaders/render_texture.vert","assets/shaders/render_texture.frag");
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
        "assets/images/skybox/right.jpg",
        "assets/images/skybox/left.jpg",
        "assets/images/skybox/top.jpg",
        "assets/images/skybox/bottom.jpg",
        "assets/images/skybox/front.jpg",
        "assets/images/skybox/back.jpg",
    };
    skyboxCubemap = textureManager.LoadCubemap(skyboxFaces);
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


void Nullity::WindowSizeCallback(GLFWwindow* window, int width, int height)
{
    Engine* eng = static_cast<Engine*>(glfwGetWindowUserPointer(window));

    State* state = &eng->state;

    // letterbox scaling
    float aspect = (float)width / height;
    float targetAspect = (float)state->initViewRes.x / (float)state->initViewRes.y;

    if (aspect > targetAspect)
    {
        state->viewRes.y = height;
        state->viewRes.x = (int)(height * targetAspect);
    }
    else
    {
        state->viewRes.x = width;
        state->viewRes.y = (int)(width / targetAspect);
    }

    state->viewOffset.x = (width - state->viewRes.x) / 2;
    state->viewOffset.y = (height - state->viewRes.y) / 2;

    glViewport(state->viewOffset.x, state->viewOffset.y, state->viewRes.x, state->viewRes.y);

    eng->framebuffer.Refresh(state->viewRes.x, state->viewRes.y);
}
