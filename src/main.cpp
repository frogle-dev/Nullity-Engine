#include "../lib/glad.h"
#include <GLFW/glfw3.h>
#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/type_ptr.hpp"

#include "../lib/imgui/imgui.h"
#include "../lib/imgui/backends/imgui_impl_glfw.h"
#include "../lib/imgui/backends/imgui_impl_opengl3.h"

#include "shader.hpp"
#include "primitives.hpp"
#include "keymap.hpp"
#include "helpers.hpp"
#include "camera.hpp"
#include "textures.hpp"
#include "models.hpp"
#include "framebuffer.h"

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
    // initialization
    GLFWwindow* window;
    if (!init(window, initWidth, initHeight))
        return -1;
    
    // framebuffer
    Framebuffer gameFrameBuffer(viewWidth, viewHeight);
    glfwSetWindowSizeCallback(window, window_size_callback);

    glfwSetCursorPosCallback(window, mouse_callback);
    glfwSetKeyCallback(window, key_callback);

    // imgui init
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO();
    io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
    io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable; 
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
    
    // imgui styling
    ImGuiStyle& style = ImGui::GetStyle();
    ImVec4 pink = ImVec4(0.725f, 0.451f, 0.459f, 1.0f);
    ImVec4 gray = ImVec4(0.176f, 0.176f, 0.204f, 1.0f);
    ImVec4 dark_gray = ImVec4(0.155f, 0.155f, 0.183f, 1.0f);
    ImVec4 light_pink = ImVec4(0.808f, 0.694f, 0.745f, 1.0f);
    if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
    {
        style.WindowRounding = 5.0f;
        style.ChildRounding = 2.5f;
        style.FrameRounding = 2.5f;
        style.TabBarOverlineSize = 0.0f;
        
        // windows
        style.Colors[ImGuiCol_TitleBg] = dark_gray;
        style.Colors[ImGuiCol_TitleBgActive] = dark_gray;
        style.Colors[ImGuiCol_TabActive] = light_pink;
        style.Colors[ImGuiCol_TabHovered] = light_pink;
        style.Colors[ImGuiCol_TabDimmed] = pink;
        style.Colors[ImGuiCol_TabDimmedSelected] = pink;
        style.Colors[ImGuiCol_Tab] = pink;
        style.Colors[ImGuiCol_TabUnfocused] = pink;
        style.Colors[ImGuiCol_TabUnfocusedActive] = pink;
        style.Colors[ImGuiCol_TabSelected] = light_pink;
        style.Colors[ImGuiCol_WindowBg] = gray;
        style.Colors[ImGuiCol_PopupBg] = gray;
        style.Colors[ImGuiCol_Border] = pink;
        style.Colors[ImGuiCol_ResizeGrip] = pink;
        style.Colors[ImGuiCol_ResizeGripActive] = light_pink;
        style.Colors[ImGuiCol_ResizeGripHovered] = light_pink;
        
        // menu bar
        style.Colors[ImGuiCol_MenuBarBg] = dark_gray;
        style.Colors[ImGuiCol_Header] = pink;
        style.Colors[ImGuiCol_HeaderHovered] = light_pink;
        style.Colors[ImGuiCol_HeaderActive] = light_pink;

        // list
        style.Colors[ImGuiCol_FrameBg] = dark_gray;

        // buttons
        style.Colors[ImGuiCol_Button] = pink;
        style.Colors[ImGuiCol_ButtonActive] = light_pink;
        style.Colors[ImGuiCol_ButtonHovered] = light_pink;
    }
    
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init("#version 460");

    // setup keybinds from json file
    std::string cur_actionName;
    std::vector<int> cur_keycodes;
    reloadConfigKeymaps();
    
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

    
    // rendering and shader stuff
    Shader objectShader("/home/jonah/Programming/Opengl/opengl-first-project/shaders/lit.vert", "/home/jonah/Programming/Opengl/opengl-first-project/shaders/lit.frag");
    Shader lightSourceShader("/home/jonah/Programming/Opengl/opengl-first-project/shaders/light_source.vert", "/home/jonah/Programming/Opengl/opengl-first-project/shaders/light_source.frag");
    Shader skyboxShader("/home/jonah/Programming/Opengl/opengl-first-project/shaders/skybox.vert", "/home/jonah/Programming/Opengl/opengl-first-project/shaders/skybox.frag");
    Shader instancedShader("/home/jonah/Programming/Opengl/opengl-first-project/shaders/instanced.vert", "/home/jonah/Programming/Opengl/opengl-first-project/shaders/instanced.frag"); 
    Shader grassShader("/home/jonah/Programming/Opengl/opengl-first-project/shaders/grass.vert", "/home/jonah/Programming/Opengl/opengl-first-project/shaders/grass.frag");

    // light block setup
    GLuint lightVAO, VBO;
    glGenVertexArrays(1, &lightVAO);
    glGenBuffers(1, &VBO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices_cube), &vertices_cube, GL_STATIC_DRAW);
    glBindVertexArray(lightVAO);
    setLightSourceVertAttribs();

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

    glEnable(GL_BLEND); 
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

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
    

    GLuint dirtTexture = TextureManager::Get().LoadStandaloneTexture("/home/jonah/Programming/Opengl/opengl-first-project/images/dirt.png");

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
    registry.emplace<Transform>(dirt, glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f), glm::vec3(0.01f));
    registry.emplace<WorldObject>(dirt, objectShader);
    registry.emplace<ObjectModel>(dirt, Model("../models/Windfall/Windfall.obj"), true);

    auto player = registry.create();
    registry.emplace<Transform>(player);
    registry.emplace<Player>(player);
    registry.emplace<Velocity>(player);


    // imgui stuff
    bool demoWindow = false;
    bool changingKeybind = false;

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
        
        // info panel for fps/ms per frame and keymaps
        ImGui::Begin("Info", NULL, ImGuiWindowFlags_None);
        ImGui::Text("ms per frame: %f", msPerFrame);
        ImGui::Text("fps: %i", fps);
        
        ImGui::Separator();
        ImGui::Text("Keymaps");
        ImGui::BeginChild("Keymaps");
        auto& bindings = getConfigKeymaps();
        for (auto& [actionName, keycodes] : bindings)
        {
            if (ImGui::Button(actionName.c_str()))
            {
                cur_actionName = actionName;
                changingKeybind = true;
                ImGui::OpenPopup("Change Keymap?");
            }
        }
        if (changingKeybind)
        {
            cur_keycodes = bindings[cur_actionName];
        }

        if(ImGui::BeginPopupModal("Change Keymap?", NULL, ImGuiWindowFlags_AlwaysAutoResize))
        {
            ImGui::Text("Changing action: %s", cur_actionName.c_str());
            ImGui::Text("Press a key, then click 'add' or 'change' to assign the currently pressed key to that slot");
            
            ImGui::Separator();
            ImGui::Text("Press any key: %i", currentScancodePress);

            if(ImGui::BeginListBox("Current assigned keycodes"))
            {
                for (int i = 0; i < cur_keycodes.size(); i++)
                {
                    int key = cur_keycodes[i];
                    ImGui::Text("%i", key);
                    ImGui::SameLine();

                    ImGui::PushID(key + i);
                    if (ImGui::Button("Change"))
                    {
                        setConfigKeymap(cur_actionName, false, currentScancodePress, i);
                        reloadConfigKeymaps();
                    }
                    ImGui::SameLine();
                    if (ImGui::Button("Remove"))
                    {
                        removeConfigKeymap(cur_actionName, i);
                        reloadConfigKeymaps();
                    }
                    ImGui::PopID();
                }
                if(ImGui::Button("Add"))
                {
                    setConfigKeymap(cur_actionName, true, currentScancodePress);
                    reloadConfigKeymaps();
                }
                ImGui::EndListBox();
            }

            if (ImGui::Button("Close")) 
            { 
                ImGui::CloseCurrentPopup(); 
                changingKeybind = false;
            }
            ImGui::EndPopup();
        }
        ImGui::EndChild();
        ImGui::End();
        
        // inspector window for modifying properties of objects
        ImGui::Begin("Inspector", NULL, ImGuiWindowFlags_None);
        ImGui::Text("Entity: ");
        ImGui::End();

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
        PlayerUpdate(registry, camera, deltaTime);


        objectShader.use();
        // lighting
        objectShader.setVec3("viewPos", camera.position);


        TextureManager::Get().SendSubTexResArrayToShader(texArrayDataUBO); // send the tex res array to the frag shader

        // rendering
        glm::mat4 view = camera.GetViewMatrix();
        glm::mat4 projection = glm::perspective(glm::radians(80.0f), viewWidth / (float)viewHeight, 0.1f, 1000.0f);

        SetUniformBufferData(matricesUBO, 0, 64, glm::value_ptr(view));
        SetUniformBufferData(matricesUBO, 64, 64, glm::value_ptr(projection));
        
        // drawing scene
        WorldObjectSystem(registry);

        DrawSystem(registry);


        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_CUBE_MAP, skyboxCubemap); // binding skybox for reflections

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
    // glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    glDeleteBuffers(1, &lightVAO);
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

    // if (focus)
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