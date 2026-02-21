#pragma once

#include "glad.h"
#include <GLFW/glfw3.h>

#include <entt/entt.hpp>

#include "shader.hpp"


namespace Nullity 
{
    struct State
    {
        const glm::ivec2 initViewRes = glm::ivec2(1280, 720); 
        glm::ivec2 viewRes = initViewRes;
        glm::ivec2 viewOffset = glm::ivec2(0, 0);

        bool wireframe = false;
        bool focus = true;
    };

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
        Shader renderTexShader;

        // uniform buffers
        GLuint matricesUBO;
        GLuint texArrayDataUBO;
        GLuint skyboxVAO, skyboxVBO;
        GLuint skyboxCubemap;

        void InitShaders();
        void InitUBOs();
        void InitSkybox();

        void Cleanup();
    };
}
