#define STB_IMAGE_IMPLEMENTATION

#include "glad.h"
#include <GLFW/glfw3.h>
#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/type_ptr.hpp"

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

    Camera camera;
    glm::vec2 lastMousePos = initViewRes / 2;

    EngineState engineState;
};


int main()
{
    AppState appState;
    appState.engineState = EngineState();

    GLFWwindow* window;
    if (!init(window, appState.initViewRes.x, appState.initViewRes.y))
        return -1;
 
    glfwSetWindowUserPointer(window, &appState);

    // framebuffer
    Framebuffer gameFrameBuffer(appState.viewRes.x, appState.viewRes.y);
    glfwSetWindowSizeCallback(window, window_size_callback);

    glfwSetCursorPosCallback(window, mouse_callback);
    glfwSetKeyCallback(window, key_callback);


    ImguiInit(window);
    ImGuiIO& io = ImGui::GetIO();

    float accent1[4] = {251.0f/255, 103.0f/255, 255.0f/255, 255.0f/255};
    float accent2[4] = {251.0f/255, 103.0f/255, 255.0f/255, 100.0f/255};
    float bg1[4] = {60.0f/255, 60.0f/255, 60.0f/255, 255.0f/255};
    float bg2[4] = {0.0f/255, 0.0f/255, 0.0f/255, 84.0f/255};
    
    // setup keybinds from json file
    reloadConfigKeymaps();


    Shader objectShader("/home/jonah/Programming/Opengl/opengl-first-project/shaders/lit.vert", "/home/jonah/Programming/Opengl/opengl-first-project/shaders/lit.frag");
    Shader lightSourceShader("/home/jonah/Programming/Opengl/opengl-first-project/shaders/light_source.vert", "/home/jonah/Programming/Opengl/opengl-first-project/shaders/light_source.frag");
    Shader skyboxShader("/home/jonah/Programming/Opengl/opengl-first-project/shaders/skybox.vert", "/home/jonah/Programming/Opengl/opengl-first-project/shaders/skybox.frag");
    Shader instancedShader("/home/jonah/Programming/Opengl/opengl-first-project/shaders/instanced.vert", "/home/jonah/Programming/Opengl/opengl-first-project/shaders/instanced.frag"); 
    Shader grassShader("/home/jonah/Programming/Opengl/opengl-first-project/shaders/grass.vert", "/home/jonah/Programming/Opengl/opengl-first-project/shaders/grass.frag");
    Shader unlitShader("/home/jonah/Programming/Opengl/opengl-first-project/shaders/unlit.vert", "/home/jonah/Programming/Opengl/opengl-first-project/shaders/unlit.frag");

    // uniform buffers
    GLuint matricesUBO;
    glGenBuffers(1, &matricesUBO);
    glBindBuffer(GL_UNIFORM_BUFFER, matricesUBO);
    glBufferData(GL_UNIFORM_BUFFER, 128, NULL, GL_STATIC_DRAW);
    glBindBufferBase(GL_UNIFORM_BUFFER, 0, matricesUBO);
    glBindBuffer(GL_UNIFORM_BUFFER, 0);

    GLuint texArrayDataUBO;
    glGenBuffers(1, &texArrayDataUBO);
    glBindBuffer(GL_UNIFORM_BUFFER, texArrayDataUBO);
    glBufferData(GL_UNIFORM_BUFFER, 1616, NULL, GL_STATIC_DRAW);
    glBindBufferBase(GL_UNIFORM_BUFFER, 1, texArrayDataUBO);
    glBindBuffer(GL_UNIFORM_BUFFER, 0);

    
    GLuint skyboxVAO, skyboxVBO;
    glGenVertexArrays(1, &skyboxVAO);
    glGenBuffers(1, &skyboxVBO);
    glBindVertexArray(skyboxVAO);
    glBindBuffer(GL_ARRAY_BUFFER, skyboxVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(skyboxVertices), &skyboxVertices, GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);

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

    objectShader.use();
    TextureManager::Get().GenerateTextureArray(4096, 4096, 100, texArrayDataUBO);
    
    GLuint texArrayID = TextureManager::Get().GetTexArrayID();

    objectShader.setFloat("material.emissionStrength", 1.0f);
    objectShader.setFloat("material.shininess", 128.0f);

    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

    TextureManager::Get().GenerateMipmaps(); // generate texture array mipmaps once all textures have been loaded in
    TextureManager::Get().SendSubTexResArrayToShader(texArrayDataUBO); // send the tex res array to the frag shader
    

    skyboxShader.use();
    std::vector<std::string> skyboxFaces = {
        "../images/skybox/right.jpg",
        "../images/skybox/left.jpg",
        "../images/skybox/top.jpg",
        "../images/skybox/bottom.jpg",
        "../images/skybox/front.jpg",
        "../images/skybox/back.jpg",
    };
    GLuint skyboxCubemap = TextureManager::Get().LoadCubemap(skyboxFaces);

    // object loading
    entt::registry registry;

    auto dirt = registry.create();
    registry.emplace<DisplayName>(dirt, "dirt");
    registry.emplace<Transform>(dirt, glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f), glm::vec3(0.01f));
    registry.emplace<WorldObject>(dirt, unlitShader);
    registry.emplace<ObjectModel>(dirt, Model("../models/Windfall/Windfall.obj"), true);

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
        UtilityKeybinds(window, appState.engineState);
        PlayerUpdate(registry, appState.camera, deltaTime);


        objectShader.use();
        objectShader.setVec3("viewPos", appState.camera.position);

        TextureManager::Get().SendSubTexResArrayToShader(texArrayDataUBO); // send the tex res array to the frag shader

        // rendering
        glm::mat4 view = appState.camera.GetViewMatrix();
        glm::mat4 projection = glm::perspective(glm::radians(80.0f), (float)appState.viewRes.x / appState.viewRes.y, 0.1f, 1000.0f);

        SetUniformBufferData(matricesUBO, 0, 64, glm::value_ptr(view));
        SetUniformBufferData(matricesUBO, 64, 64, glm::value_ptr(projection));
        
        // drawing scene
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_CUBE_MAP, skyboxCubemap); // binding skybox for reflections

        WorldObjectSystem(registry);
        DrawSystem(registry);

        // skybox
        glDepthFunc(GL_LEQUAL);
        skyboxShader.use();
        view = glm::mat4(glm::mat3(appState.camera.GetViewMatrix()));

        SetUniformBufferData(matricesUBO, 0, 64, glm::value_ptr(view));

        glBindVertexArray(skyboxVAO);
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
    gameFrameBuffer.Cleanup();
    objectShader.deleteProgram();
    lightSourceShader.deleteProgram();

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


bool firstMouse = true;
void mouse_callback(GLFWwindow* window, double xpos, double ypos)
{
    AppState* state = static_cast<AppState*>(glfwGetWindowUserPointer(window));

    if (firstMouse)
    {
        state->lastMousePos.x = xpos;
        state->lastMousePos.y = ypos;
        firstMouse = false;
    } // this is so when mouse initially moves, it doesnt make a large jkittery motion to that position

    if (state->engineState.focus)
    {
        float xOffset = xpos - state->lastMousePos.x;
        float yOffset = state->lastMousePos.y - ypos;
        state->lastMousePos.x = xpos;
        state->lastMousePos.y = ypos;
    
        state->camera.ProcessMouseMovement(xOffset, yOffset);
    }
}

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
    processKeyEvent(scancode, action);
}