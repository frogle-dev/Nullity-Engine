#define STB_IMAGE_IMPLEMENTATION

#include "glad.h"
#include <GLFW/glfw3.h>
#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/type_ptr.hpp"

#include "FastNoiseLite.h"

#include "imgui/imgui.h"
#include "imgui/backends/imgui_impl_glfw.h"
#include "imgui/backends/imgui_impl_opengl3.h"

#include "shader.hpp"
#include "primitives.hpp"
#include "keymap.hpp"
#include "camera.hpp"
#include "textures.hpp"
#include "models.hpp"
#include "framebuffer.hpp"

#include "init.hpp"
#include "utility.hpp"
#include "inspector.hpp"
#include "engine_gui.hpp"
#include "systems.hpp"
#include "player.hpp"
#include "render.hpp"

#include <algorithm>
#include <cmath>
#include <iostream>


void window_size_callback(GLFWwindow* window, int width, int height);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods);


struct AppState
{
    const glm::ivec2 initViewRes = glm::ivec2(1280, 720); 
    glm::ivec2 viewRes = initViewRes;

    EngineState* engineState;
    MouseState* mouseState;
};


int main()
{
    AppState App;
    EngineState engineState;
    MouseState mouseState;
    App.engineState = &engineState;
    App.mouseState = &mouseState;

    mouseState.lastMousePos = App.initViewRes / 2;


    GLFWwindow* window;
    if (!init(window, App.initViewRes.x, App.initViewRes.y))
        return -1;
 
    glfwSetWindowUserPointer(window, &App);

    // framebuffer
    Framebuffer gameFrameBuffer(App.viewRes.x, App.viewRes.y);
    glfwSetWindowSizeCallback(window, window_size_callback);

    glfwSetCursorPosCallback(window, mouse_callback);
    glfwSetKeyCallback(window, key_callback);


    ImguiInit(window);
    ImGuiIO& io = ImGui::GetIO();

    float accent1[4] = {251.0f/255, 103.0f/255, 255.0f/255, 255.0f/255};
    float accent2[4] = {251.0f/255, 103.0f/255, 255.0f/255, 100.0f/255};
    float bg1[4] = {60.0f/255, 60.0f/255, 60.0f/255, 255.0f/255};
    float bg2[4] = {0.0f/255, 0.0f/255, 0.0f/255, 84.0f/255};


    EngineData Engine;

    // setup keybinds from json file
    reloadConfigKeymaps();

    // instancing
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

    // enable depth testing and face culling
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);

    // glEnable(GL_BLEND); 
    // glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    // texture stuff
    glActiveTexture(GL_TEXTURE0);

    Engine.objectShader.use();
    TextureManager::Get().GenerateTextureArray(4096, 4096, 100, Engine.texArrayDataUBO);
    
    GLuint texArrayID = TextureManager::Get().GetTexArrayID();

    Engine.objectShader.setFloat("material.emissionStrength", 1.0f);
    Engine.objectShader.setFloat("material.shininess", 128.0f);

    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

    TextureManager::Get().GenerateMipmaps(); // generate texture array mipmaps once all textures have been loaded in
    TextureManager::Get().SendSubTexResArrayToShader(Engine.texArrayDataUBO); // send the tex res array to the frag shader
    

    Engine.skyboxShader.use();
    std::vector<std::string> skyboxFaces = {
        "../images/skybox/right.jpg",
        "../images/skybox/left.jpg",
        "../images/skybox/top.jpg",
        "../images/skybox/bottom.jpg",
        "../images/skybox/front.jpg",
        "../images/skybox/back.jpg",
    };
    GLuint skyboxCubemap = TextureManager::Get().LoadCubemap(skyboxFaces);


    FastNoiseLite noise;
    noise.SetNoiseType(FastNoiseLite::NoiseType_OpenSimplex2);
    noise.SetFractalType(FastNoiseLite::FractalType_FBm);
    std::vector<float> noiseData(128 * 128);
    std::vector<glm::mat4> mats;
    int index = 0;

    for (int y = 0; y < 128; y++)
    {
        for (int x = 0; x < 128; x++)
        {
            noiseData.push_back( noise.GetNoise((float)x, (float)y));
            glm::mat4 model = glm::mat4(0.0f);
            model = glm::translate(model, glm::vec3((float)x, 0.0f, (float)y));
            model = glm::scale(model, glm::vec3(1.0f));
            mats.push_back(model);
            index++;
        }
    }


    // object loading
    entt::registry registry;

    auto dirt = registry.create();
    registry.emplace<DisplayName>(dirt, "dirt");
    registry.emplace<ObjectShader>(dirt, Engine.instancedShader);
    registry.emplace<ObjectModel>(dirt, Model("../models/Dirt/Dirt.obj", mats.size(), mats), true);

    auto player = registry.create();
    registry.emplace<DisplayName>(player, "player");
    registry.emplace<Transform>(player);
    registry.emplace<Player>(player);
    registry.emplace<Velocity>(player);


    // imgui stuff
    bool demoWindow = false;

    gameFrameBuffer.Unbind();

    // render loop
    float deltaTime = 0.0f;
    int fps;
    float msPerFrame;

    float lastFrame = 0.0f;
    while(!glfwWindowShouldClose(window))
    {
        float currentFrame = glfwGetTime();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;
        
        msPerFrame = deltaTime * 1000;
        fps = 1000 / msPerFrame;


        // imgui
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();
        ImGui::DockSpaceOverViewport(0, ImGui::GetMainViewport(), ImGuiDockNodeFlags_PassthruCentralNode);

        if (ImGui::BeginMainMenuBar())
        {
            if (ImGui::BeginMenu("Window"))
            {
                if (ImGui::MenuItem("Demo Window")) 
                {
                    demoWindow = !demoWindow;
                }

                ImGui::EndMenu();
            }
            ImGui::EndMainMenuBar();
        }

        if (demoWindow)
        {
            ImGui::ShowDemoWindow();
        }
        
        InfoWindow(msPerFrame, fps);
        DebugOutputWindow();
        InspectorWindow(registry);

        ImGui::Begin("Game");
        {
            ImGui::BeginChild("Render");
            
            float width = ImGui::GetContentRegionAvail().x;
            float height = ImGui::GetContentRegionAvail().y;
            
            gameFrameBuffer.Rescale(width, height);
            window_size_callback(window, width, height);

            ImGui::Image(
                (ImTextureID)gameFrameBuffer.GetColorTexture(), 
                ImGui::GetContentRegionAvail(), 
                ImVec2(0, 1), 
                ImVec2(1, 0)
            );
        }
        ImGui::EndChild();
        ImGui::End();

        ImGui::Begin("Style");
        ImGui::ColorEdit4("accent1", accent1);
        ImGui::ColorEdit4("accent2", accent2);
        ImGui::ColorEdit4("bg1", bg1);
        ImGui::ColorEdit4("bg2", bg2);
        Styling(accent1, accent2, bg1, bg2);
        ImGui::End();
        // imgui rendering
        ImGui::Render();

        gameFrameBuffer.Bind();
        glClearColor(0.2f, 0.3f, 0.6f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); //clear color + depth buffer
        glEnable(GL_DEPTH_TEST);


        // game loop stuff
        UtilityKeybinds(window, engineState);
        PlayerUpdate(registry, mouseState.camera, deltaTime);
        CameraControls(mouseState, engineState);


        Engine.objectShader.use();
        Engine.objectShader.setVec3("viewPos", mouseState.camera.position);

        TextureManager::Get().SendSubTexResArrayToShader(Engine.texArrayDataUBO); // send the tex res array to the frag shader

        // rendering
        glm::mat4 view = mouseState.camera.GetViewMatrix();
        glm::mat4 projection = glm::perspective(glm::radians(80.0f), (float)App.viewRes.x / App.viewRes.y, 0.1f, 1000.0f);

        SetUniformBufferData(Engine.matricesUBO, 0, 64, glm::value_ptr(view));
        SetUniformBufferData(Engine.matricesUBO, 64, 64, glm::value_ptr(projection));
        
        // drawing scene
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_CUBE_MAP, skyboxCubemap); // binding skybox for reflections

        WorldObjectSystem(registry);
        DrawSystem(registry);

        // skybox
        glDepthFunc(GL_LEQUAL);
        Engine.skyboxShader.use();
        view = glm::mat4(glm::mat3(mouseState.camera.GetViewMatrix()));

        SetUniformBufferData(Engine.matricesUBO, 0, 64, glm::value_ptr(view));

        glBindVertexArray(Engine.skyboxVAO);
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_CUBE_MAP, skyboxCubemap);
        glDrawArrays(GL_TRIANGLES, 0, 36);
        glDepthFunc(GL_LESS);

        glBindVertexArray(0);
        
        // game rendering finished, now render texture quad rendering
        gameFrameBuffer.Unbind();
        glClearColor(0.2f, 0.3f, 0.6f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT); //clear color + depth buffer
        

        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
        if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
        {
            GLFWwindow* backup_current_context = glfwGetCurrentContext();
            ImGui::UpdatePlatformWindows();
            ImGui::RenderPlatformWindowsDefault();
            glfwMakeContextCurrent(backup_current_context);
        }

        glfwSwapBuffers(window);
        glfwPollEvents();
    }
    
    // end of process life
    Engine.Cleanup();
    gameFrameBuffer.Cleanup();

    // imgui
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();

    glfwTerminate();
    return 0;
}


void window_size_callback(GLFWwindow* window, int width, int height)
{
    AppState* state = static_cast<AppState*>(glfwGetWindowUserPointer(window));

    // letterbox scaling
    float aspect = (float)width / height;
    float targetAspect = (float)state->initViewRes.x / state->initViewRes.y;

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

    int viewX = (width - state->viewRes.x) / 2;
    int viewY = (height - state->viewRes.y) / 2;

    glViewport(viewX, viewY, state->viewRes.x, state->viewRes.y);
}


void mouse_callback(GLFWwindow* window, double xpos, double ypos)
{
    AppState* state = static_cast<AppState*>(glfwGetWindowUserPointer(window));

    state->mouseState->mousePos = glm::dvec2(xpos, ypos);
}

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
    processKeyEvent(scancode, action);
}