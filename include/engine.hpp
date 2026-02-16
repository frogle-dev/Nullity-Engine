#pragma once

#include "glad.h"
#include <GLFW/glfw3.h>

#include <entt/entt.hpp>

#include "shader.hpp"

namespace Engine 
{
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

    class Scene
    {
    public:
        entt::entity dirt;
        entt::entity player;

        void LoadObjects(Data&);
    };
}
