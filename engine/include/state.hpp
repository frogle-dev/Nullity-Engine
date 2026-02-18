#pragma once

#include "glad.h"
#include <GLFW/glfw3.h>

#include <entt/entt.hpp>

#include "shader.hpp"


namespace Nullity 
{
    struct MouseState
    {
        glm::vec2 lastMousePos;
        bool firstMouse = true;
        glm::dvec2 mousePos;
    };

    struct State
    {
        const glm::ivec2 initViewRes = glm::ivec2(1920, 1080); 
        glm::ivec2 viewRes = initViewRes;

        bool focus = true;
        bool wireframe = false;

        float deltaTime = 0.0f;
        int fps;
        float msPerFrame;
        float lastFrame = 0.0f;

        MouseState* mouse;
    };


    void UpdateEngine(State& engState);
    void UtilityKeybinds(GLFWwindow* window, State& engineState);


    class Data
    {
    public:
        Data();

        Shader objectShader = Shader("shaders/lit.vert", "shaders/lit.frag");
        Shader lightSourceShader = Shader("shaders/light_source.vert", "shaders/light_source.frag");
        Shader skyboxShader = Shader("shaders/skybox.vert", "shaders/skybox.frag");
        Shader instancedShader = Shader("shaders/instanced.vert", "shaders/instanced.frag"); 
        Shader grassShader = Shader("shaders/grass.vert", "shaders/grass.frag");
        Shader unlitShader = Shader("shaders/unlit.vert", "shaders/unlit.frag");

        // uniform buffers
        GLuint matricesUBO;
        GLuint texArrayDataUBO;
        GLuint skyboxVAO, skyboxVBO;
        GLuint skyboxCubemap;

        entt::registry registry;

        void InitUBOs();
        void InitSkybox();

        void Cleanup();
    };
}
