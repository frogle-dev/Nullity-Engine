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

#include "initialization.hpp"
#include "utility.hpp"
#include "inspector.hpp"
#include "gui.hpp"
#include "systems.hpp"
#include "player.hpp"
#include "render.hpp"

#include <algorithm>
#include <cmath>
#include <iostream>


void window_size_callback(GLFWwindow* window, int width, int height);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods);

const int initWidth = 1280, initHeight = 720;
int viewWidth = initWidth, viewHeight = initHeight;

Camera camera;
float lastMouseX = initWidth/2, lastMouseY = initHeight/2;

int main()
{
    GLFWwindow* window;
    if (!init(window, initWidth, initHeight))
        return -1;
    
    // framebuffer
    Framebuffer gameFrameBuffer(viewWidth, viewHeight);
    glfwSetWindowSizeCallback(window, window_size_callback);

    glfwSetCursorPosCallback(window, mouse_callback);
    glfwSetKeyCallback(window, key_callback);


    ImguiInit(window);
ImGuiIO& io = ImGui::GetIO();
    
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
        // imgui rendering
        ImGui::Render();

        gameFrameBuffer.Bind();
        glClearColor(0.2f, 0.3f, 0.6f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); //clear color + depth buffer
        glEnable(GL_DEPTH_TEST);


        // game loop stuff
        UtilityKeybinds(window);
        PlayerUpdate(registry, camera, deltaTime);


        objectShader.use();
        objectShader.setVec3("viewPos", camera.position);

        TextureManager::Get().SendSubTexResArrayToShader(texArrayDataUBO); // send the tex res array to the frag shader

        // rendering
        glm::mat4 view = camera.GetViewMatrix();
        glm::mat4 projection = glm::perspective(glm::radians(80.0f), viewWidth / (float)viewHeight, 0.1f, 1000.0f);

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
        view = glm::mat4(glm::mat3(camera.GetViewMatrix()));

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
    // letterbox scaling
    float aspect = (float)width / height;
    float targetAspect = (float)initWidth / initHeight;

    if (aspect > targetAspect)
    {
        viewHeight = height;
        viewWidth = (int)(height * targetAspect);
    }
    else
    {
        viewWidth = width;
        viewHeight = (int)(width / targetAspect);
    }

    int viewX = (width - viewWidth) / 2;
    int viewY = (height - viewHeight) / 2;

    glViewport(viewX, viewY, viewWidth, viewHeight);
}


bool firstMouse = true;
void mouse_callback(GLFWwindow* window, double xpos, double ypos)
{
    if (firstMouse)
    {
        lastMouseX = xpos;
        lastMouseY = ypos;
        firstMouse = false;
    } // this is so when mouse initially moves, it doesnt make a large jkittery motion to that position

    if (focus)
    {
        float xOffset = xpos - lastMouseX;
        float yOffset = lastMouseY - ypos;
        lastMouseX = xpos;
        lastMouseY = ypos;
    
        camera.ProcessMouseMovement(xOffset, yOffset);
    }
}

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
    processKeyEvent(scancode, action);
}