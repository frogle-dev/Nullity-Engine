#pragma once

#include "glad.h"
#include <GLFW/glfw3.h>

#include <iostream>
#include <string>

#include "shader.hpp"
#include "debugging.hpp"

#include "imgui/imgui.h"
#include "imgui/backends/imgui_impl_glfw.h"
#include "imgui/backends/imgui_impl_opengl3.h"


class EngineData
{
public:
    EngineData();

    Shader objectShader = Shader("/home/jonah/Programming/Opengl/opengl-first-project/shaders/lit.vert", "/home/jonah/Programming/Opengl/opengl-first-project/shaders/lit.frag");
    Shader lightSourceShader = Shader("/home/jonah/Programming/Opengl/opengl-first-project/shaders/light_source.vert", "/home/jonah/Programming/Opengl/opengl-first-project/shaders/light_source.frag");
    Shader skyboxShader = Shader("/home/jonah/Programming/Opengl/opengl-first-project/shaders/skybox.vert", "/home/jonah/Programming/Opengl/opengl-first-project/shaders/skybox.frag");
    Shader instancedShader = Shader("/home/jonah/Programming/Opengl/opengl-first-project/shaders/instanced.vert", "/home/jonah/Programming/Opengl/opengl-first-project/shaders/instanced.frag"); 
    Shader grassShader = Shader("/home/jonah/Programming/Opengl/opengl-first-project/shaders/grass.vert", "/home/jonah/Programming/Opengl/opengl-first-project/shaders/grass.frag");
    Shader unlitShader = Shader("/home/jonah/Programming/Opengl/opengl-first-project/shaders/unlit.vert", "/home/jonah/Programming/Opengl/opengl-first-project/shaders/unlit.frag");

    // uniform buffers
    GLuint matricesUBO;
    GLuint texArrayDataUBO;
    GLuint skyboxVAO, skyboxVBO;

    void InitUBOs();
    void InitSkybox();

    void Cleanup();
};


bool init(GLFWwindow *&windowID, int width, int height);

void ImguiInit(GLFWwindow* window);
