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

        MouseState mouse;
    };

    void UtilityKeybinds(GLFWwindow* window, State& engineState);

    class Data
    {
    public:
        void InitData();

        Shader objectShader;
        Shader lightSourceShader;
        Shader skyboxShader;
        Shader instancedShader;
        Shader grassShader;
        Shader unlitShader;

        // uniform buffers
        GLuint matricesUBO;
        GLuint texArrayDataUBO;
        GLuint skyboxVAO, skyboxVBO;
        GLuint skyboxCubemap;

        entt::registry registry;

        void InitShaders();
        void InitUBOs();
        void InitSkybox();

        void Cleanup();
    };
}
